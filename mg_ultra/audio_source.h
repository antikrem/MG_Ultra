/*Encapsulates a source of audio
*/
#ifndef __AUDIO_SOURCE__
#define __AUDIO_SOURCE__

#include <mutex>	
#include <map>

#include "error.h"

#include "audio_file.h"
#include "cus_struct2.h"

//forward declaraction of disposition function
//implementation in audio_master.cpp
namespace g_audio {
	void disposeSource(ALint buffer);

	//destroys g audio master 
	void closeAudioPipeLine();
}

class AudioSource {
private:
	mutex lock;

	bool initialised = false;

	bool resetOnDoubleCall = false;

	//when set to >= 0, indicates request for audio repeat change
	int requestedRepeat = -1;

	//when set to >= 0, indicates request for audio gain change
	float requestedGain = -1.0f;


	string currentAudio;
	string requestedAudio;

	bool requestPlay = false;
	bool requestStop = false;


	//id of source
	ALuint sourceID;
public:
	//On destruction, sends out a disposal request
	//to AL handle thread
	~AudioSource() {
		unique_lock<mutex> lck(lock);
		if (initialised) {
			g_audio::disposeSource(sourceID);
		}
	}

	//sets a sound file as what will be played
	void setQueuedAudio(const string& audioName) {
		unique_lock<mutex> lck(lock);
		requestedAudio = audioName;
	}

	//played the queued audio
	void playQueuedAudio() {
		unique_lock<mutex> lck(lock);
		requestPlay = true;
	}

	//plays a sound through this audio source
	void playAudio(const string& audioName) {
		setQueuedAudio(audioName);
		playQueuedAudio();
	}

	//stops audio
	void stopAudio() {
		unique_lock<mutex> lck(lock);
		requestStop = true;
	}

	//sets this source to a certain repeat value
	void setRepeat(bool repeat) {
		unique_lock<mutex> lck(lock);
		requestedRepeat = (int)repeat;
	}

	//sets this source to a certain gain
	void setGain(float gain) {
		unique_lock<mutex> lck(lock);
		this->requestedGain = gain;
	}


	//AL side updating
	void alSideUpdate(map<string, AudioFile*>& audioFiles, const Point3& position) {
		unique_lock<mutex> lck(lock);
		if (!initialised) {
			alGenSources(1, &sourceID);
			initialised = true;
		}

		alSource3f(sourceID, AL_POSITION, position.x, position.y, position.z);

		//handle new repeat request
		if (requestedRepeat >= 0) {
			alSourcei(sourceID, AL_LOOPING, requestedRepeat);
			requestedRepeat = -1;
		}

		//handle new repeat request
		if (requestedGain >= 0) {
			alSourcef(sourceID, AL_GAIN, requestedGain);
			requestedGain = -1.0f;
		}

		//if a requested audio is in, switch to it
		if (requestedAudio.size()) {
			//check if requested audio is availible
			if (audioFiles.count(requestedAudio)) {
				//get new bufferID to play
				int bufferID = audioFiles[requestedAudio]->getBufferID();
				alSourcei(sourceID, AL_BUFFER, bufferID);
				currentAudio = requestedAudio;
			}
			else {
				err::logMessage("AUDIO: Error, could not find requested audio: " + requestedAudio);
			}

			requestedAudio = "";
		}

		//if a requested audio has come in
		if (requestPlay) {
			requestPlay = false;
			alSourcePlay(sourceID);
		}

		//if a requested stop has come in
		if (requestStop) {
			requestStop = false;
			alSourceStop(sourceID);
		}

	}
};


#endif