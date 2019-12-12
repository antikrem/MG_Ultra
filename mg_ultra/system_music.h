/*Manages all audio rescources
*/
#ifndef __SYSTEM_MUSIC__
#define __SYSTEM_MUSIC__

#include "system.h"
#include "component_audio.h"
#include "audio_master.h"

class SystemMusic : public System {
public:
	SystemMusic() {
		debugName = "s_music";

		target = SubPoolTarget(
			ETMusic
		);
	}

	//Do nothing
	void cacheHandle(shared_ptr<Entity> ent) override {
	}

	void cacheFail(EntityPool* pool) override {
		//create the camera entity
		auto newEnt = shared_ptr<Entity>(new Entity(ETMusic));
		auto newComponent = new ComponentAudio();
		newEnt->addComponent(newComponent->pullForEntity());

		pool->addEnt(newEnt, true);
		err::logMessage("MUSIC: music player not found, new music player created");
	}

};


#endif