/*Manages all audio rescources
*/
#ifndef __SYSTEM_AUDIO__
#define __SYSTEM_AUDIO__

#include "system.h"
#include "component_audio.h"
#include "component_listener.h"
#include "audio_master.h"

class SystemAudio : public System {
	//pointer to audio master
	AudioMaster* audioMaster = nullptr;

	//listener information
	Point3 listenerLocation = Point3(0, 0, 0);
	Point3 listenTarget = Point3(0, 1, 0);

public:

	SystemAudio() {
		debugName = "s_audio";

		target = SubPoolTarget(
			{ 
				SubPoolComponents::ByComponents<ComponentAudio>(),
				SubPoolComponents::ByComponents<ComponentListener>() 
			}
		);
	}

	void setAudioMaster(AudioMaster* audioMaster) {
		this->audioMaster = audioMaster;
	}

	//reclaims the audio master
	void precycle(EntityPool* pool) override {
		audioMaster->reclaimContext();
		audioMaster->updateMaster();

		ALenum code;
		if ((code = audioMaster->getLastError())) {
			string errorName;
			switch (code) {
			case AL_NO_ERROR: errorName = "AL_NO_ERROR"; break;
			case AL_INVALID_NAME: errorName = "AL_INVALID_NAME"; break;
			case AL_INVALID_ENUM: errorName = "AL_INVALID_ENUM"; break;
			case AL_INVALID_VALUE: errorName = "AL_INVALID_VALUE"; break;
			case AL_INVALID_OPERATION: errorName = "AL_INVALID_OPERATION"; break;
			case AL_OUT_OF_MEMORY: errorName = "AL_OUT_OF_MEMORY"; break;
			default: errorName = "AL_GENERIC_ERROR"; break;
			}

			err::logMessage("AUDIO, Audio subsystem recieved an error: " + errorName);
		}
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		shared_ptr<ComponentPosition> comPos = getComponent<ComponentPosition>(components);
		shared_ptr<ComponentListener> comListener = getComponent<ComponentListener>(components);
		shared_ptr<ComponentAudio> comAudio = getComponent<ComponentAudio>(components);

		if (comListener && comPos) {
			//set new position
			listenerLocation = comPos->getPosition3();
			//conduct general update
			comListener->updateListener();
		}
		
		if (comAudio) {
			//if there is a position, update according to position
			if (comPos) { 
				comAudio->alComponentHandle(audioMaster->getAudioFiles(), comPos->getPosition3() - listenerLocation);
			}
			//otherwise update for origin
			else {
				comAudio->alComponentHandle(audioMaster->getAudioFiles(), Point3(0,0,0));
			}

			comAudio->checkAndApplyDieOn(ent);
		}
		
	}
};

#endif