/*Manages all audio rescources
*/
#ifndef __SYSTEM_AUDIO__
#define __SYSTEM_AUDIO__

#include "system.h"
#include "component_audio.h"
#include "audio_master.h"

class SystemAudio : public System {
	//pointer to audio master
	AudioMaster* audioMaster = nullptr;

public:

	SystemAudio() {
		debugName = "s_audio";

		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentAudio>()
		);
	}

	void setAudioMaster(AudioMaster* audioMaster) {
		this->audioMaster = audioMaster;
	}

	//reclaims the audio master
	void precycle(EntityPool* pool) override {
		audioMaster->reclaimContext();
		audioMaster->updateMaster();
		if (audioMaster->getLastError()) {
			err::logMessage("AUDIO, Audio subsystem recieved an error");
		}
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto comAudio = getComponent<ComponentAudio>(components);
		comAudio->alComponentHandle(audioMaster->getAudioFiles(), Point3(0, 0, 0));
	}
};

#endif