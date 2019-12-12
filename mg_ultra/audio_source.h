/*Encapsulates a source of audio
*/
#ifndef __AUDIO_SOURCE__
#define __AUDIO_SOURCE__

#include <mutex>	
#include <map>

#include "audio_file.h"
#include "cus_struct2.h"

//forward declaraction of disposition function
//implementation in audio_master.cpp
namespace g_audio {
	void disposeBuffer(ALint buffer);

	//destroys g audio master 
	void closeAudioPipeLine();
}

class AudioSource {
private:
	mutex lock;

	bool initialised = false;

	string requestedAudio;

	//id of source
	ALuint sourceID;
public:
	//On destruction, sends out a disposal request
	//to AL handle thread
	~AudioSource() {
		unique_lock<mutex> lck(lock);
		if (initialised) {
			g_audio::disposeBuffer(sourceID);
		}
	}

	//plays a sound through this audio
	void playAudio(const string& audioName) {
		unique_lock<mutex> lck(lock);
		requestedAudio = audioName;
	}

	//AL side updating
	void alSideUpdate(map<string, AudioFile*>& audioFiles, const Point3& position) {
		unique_lock<mutex> lck(lock);
		if (!initialised) {
			alGenSources(1, &sourceID);
			initialised = true;
		}

		//if a requested audio is in, siwtch to it
		if (requestedAudio.size()) {
			//get new bufferID to play
			int bufferID = audioFiles[requestedAudio]->getBufferID();
			alSourcei(sourceID, AL_BUFFER, bufferID);
			alSourcePlay(sourceID);
			requestedAudio = "";
		}

	}
};


#endif