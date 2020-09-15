/*Encapsulates a source of audio
*/
#ifndef __AUDIO_SOURCE__
#define __AUDIO_SOURCE__

#include <mutex>	
#include <map>

#include "error.h"

#include "audio_file.h"
#include "cus_struct2.h"

// Forward declaraction of disposition function
// implementation in audio_master.cpp
namespace g_audio {
	void disposeSource(ALint buffer);

	// Destroys g audio master 
	void closeAudioPipeLine();
}


// Enumeration of stance an audio source can be in
enum AudioState {
	none = -1,
	stop = 0,
	play = 1,
	pause = 2
};

class AudioSource {
private:
	// Map of functions to call when 
	const map<AudioState, void(*)(ALuint)> AUDIO_STATE_TRANSISTORS 
		= {
			{AudioState::stop, alSourceStop},
			{AudioState::play, alSourcePlay},
			{AudioState::pause, alSourcePause},
		};

	mutex lock;

	bool initialised = false;

	bool resetOnDoubleCall = false;

	//when set to >= 0, indicates request for audio repeat change
	int requestedRepeat = -1;

	//when set to >= 0, indicates request for audio gain change
	float requestedGain = -1.0f;


	string currentAudio;
	string requestedAudio;


	AudioState requestedState = AudioState::none;
	AudioState currentState = AudioState::stop;

	// id of source
	ALuint sourceID;
public:
	// On destruction, sends out a disposal request
	// to AL handle thread
	~AudioSource() {
		unique_lock<mutex> lck(lock);
		if (initialised) {
			g_audio::disposeSource(sourceID);
		}
	}

	// Sets a sound file as what will be played
	void setQueuedAudio(const string& audioName) {
		unique_lock<mutex> lck(lock);
		requestedAudio = audioName;
	}

	// Played the queued audio
	void playQueuedAudio() {
		unique_lock<mutex> lck(lock);
		requestedState = AudioState::play;
	}

	// Plays a sound through this audio source
	void playAudio(const string& audioName) {
		setQueuedAudio(audioName);
		playQueuedAudio();
	}

	// Stops audio
	void stopAudio() {
		unique_lock<mutex> lck(lock);
		requestedState = AudioState::stop;
	}

	// Stops audio
	void pauseAudio() {
		unique_lock<mutex> lck(lock);
		requestedState = AudioState::pause;
	}

	// Sets this source to a certain repeat value
	void setRepeat(bool repeat) {
		unique_lock<mutex> lck(lock);
		requestedRepeat = (int)repeat;
	}

	// Sets this source to a certain gain
	void setGain(float gain) {
		unique_lock<mutex> lck(lock);
		this->requestedGain = gain;
	}

	// Gets current state
	AudioState getState() {
		unique_lock<mutex> lck(lock);
		return currentState;
	}


	// AL side updating
	void alSideUpdate(map<string, AudioFile*>& audioFiles, const Point3& position) {
		unique_lock<mutex> lck(lock);
		if (!initialised) {
			alGenSources(1, &sourceID);
			initialised = true;
		}

		alSource3f(sourceID, AL_POSITION, position.x, position.y, position.z);

		// Handle new repeat request
		if (requestedRepeat >= 0) {
			alSourcei(sourceID, AL_LOOPING, requestedRepeat);
			requestedRepeat = -1;
		}

		// Handle new repeat request
		if (requestedGain >= 0) {
			alSourcef(sourceID, AL_GAIN, requestedGain);
			requestedGain = -1.0f;
		}

		// If a requested audio is in, switch to it
		if (requestedAudio.size()) {
			// Check if requested audio is availible
			if (audioFiles.count(requestedAudio)) {
				// Get new bufferID to play
				int bufferID = audioFiles[requestedAudio]->getBufferID();
				alSourcei(sourceID, AL_BUFFER, bufferID);
				currentAudio = requestedAudio;
			}
			else {
				err::logMessage("AUDIO: Error, could not find requested audio: " + requestedAudio);
			}

			requestedAudio = "";
		}

		// Handle requests to change audio
		if (requestedState != AudioState::none) {
			currentState = requestedState;
			AUDIO_STATE_TRANSISTORS.find(requestedState)->second(sourceID);
			requestedState = AudioState::none;
		}
		

	}
};


#endif