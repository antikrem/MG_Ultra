/*Encapsulates a single piece of audio data
i.e a sound loaded from a file
*/
#ifndef __AUDIO_FILE__
#define __AUDIO_FILE__

#include <string>

#include "sndfile.h"
#include "al.h"
#include "alc.h"

#include "constants.h"

enum AudioFileLifecycle {
	AuFiLi_Global,
	AuFiLi_Campaign,
	AuFiLi_Local
};


class AudioFile {
private:
	//lifecycle of this file
	AudioFileLifecycle lifeCycle;

	//id of buffer
	ALuint bufferID;

	//location of file
	string fileLocation;

	//stores audio information
	SF_INFO fileInfo;

public:
	AudioFile(AudioFileLifecycle lifeCycle) {
		this->lifeCycle = lifeCycle;
		alGenBuffers(1, &bufferID);
	}

	//clean ups both partial and full constructions
	~AudioFile() {

	}

	// Loads audio from a file
	// returns true if file loaded correctly
	bool loadAudioFile(const string& fileLocation) {
		this->fileLocation = fileLocation;
		SNDFILE* sndfile = sf_open(fileLocation.c_str(), SFM_READ, &fileInfo);

		if (!sndfile) {
			return false;
		}
		
		int frameCount = (int)(fileInfo.frames * fileInfo.channels);
		short* data = new short[frameCount];

		sf_readf_short(sndfile, data, frameCount);

		alBufferData(
			bufferID,
			fileInfo.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
			(void*)data,
			frameCount * sizeof(short),
			fileInfo.samplerate
		);

		sf_close(sndfile);
		delete[] data;

		return true;
	}

	//returns buffer id
	ALuint getBufferID() {
		return bufferID;
	}

	//get location of file
	string getFileLocation() {
		return fileLocation;
	}
};

#endif