/*Manages all audio rescources
*/
#ifndef __SYSTEM_WEATHER__
#define __SYSTEM_WEATHER__

#include "component_particle.h"
#include "component_audio.h"

#include "system.h"

class SystemWeather : public System {
public:
	SystemWeather() {
		debugName = "s_weather";

		target = SubPoolTarget(
			ETWeather
		);
	}

	//Do nothing
	void cacheHandle(shared_ptr<Entity> ent) override {
	}

	void cacheFail(EntityPool* pool) override {
		//create the camera entity
		auto newEnt = shared_ptr<Entity>(new Entity(ETWeather));
		auto newComponent = new ComponentParticle("weather_heavyrain");
		newEnt->addComponent(newComponent->pullForEntity());
		auto newComponent1 = new ComponentAudio();
		newEnt->addComponent(newComponent1->pullForEntity());

		pool->addEnt(newEnt, true);
		err::logMessage("WEATHER: weather manager generated");
	}

};


#endif