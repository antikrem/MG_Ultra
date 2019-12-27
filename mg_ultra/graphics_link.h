/*Collection of systems master tied to graphics*/
#ifndef __GRAPHICS_LINK__
#define __GRAPHICS_LINK__

#include "registar.h"
#include "success_callback.h"
#include "graphics_master.h"
#include "systems_master.h"

#include "system_graphics.h"
#include "system_directional_lighting.h"

class GraphicsLink {
private:
	SystemsMaster* internalSystem = nullptr;
	EntityPool* pool = nullptr;
	SystemGraphics* sGraphics = nullptr;

public:
	GraphicsLink(EntityPool* pool) {
		this->pool = pool;
		internalSystem = new SystemsMaster(pool, "m_glink");
	}

	//initialises the systems
	void startGLinking(GraphicsState* gState, Registar* registar) {
		sGraphics = createSystem<SystemGraphics>(registar);
		sGraphics->setGraphicsState(gState);
		sGraphics->setCamera(gState->getCamera());
		auto sDirectionalLights = createSystem<SystemDirectionalLight>(registar);
		sDirectionalLights->setGraphicsState(gState);
	}
	
	//creates a system and return a pointer
	template <class T>
	T* createSystem(Registar* registar) {
		T* system = new T();
		system->setGRegistar(registar);
		system->seedKey(getGlobalPool());
		internalSystem->addSystem(system);
		return system;
	}

	//executes system master and waita
	void linkGraphics() {
		SuccessCallback sc;
		internalSystem->executeSystemMaster(&sc);
		sc.waitForCompletion();
	}

	//sets particle master for sGrtaphics
	void setParticleMaster(ParticleMaster* particleMaster) {
		sGraphics->setParticleMaster(particleMaster);
	}
};


#endif