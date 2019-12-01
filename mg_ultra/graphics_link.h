/*Collection of systems master tied to graphics*/
#ifndef __GRAPHICS_LINK__
#define __GRAPHICS_LINK__

#include "registar.h"
#include "success_callback.h"
#include "graphics_master.h"
#include "systems_master.h"

#include "system_graphics.h"

class GraphicsLink {
private:
	SystemsMaster* internalSystem = nullptr;
	EntityPool* pool = nullptr;

public:
	GraphicsLink(EntityPool* pool) {
		this->pool = pool;
		internalSystem = new SystemsMaster(pool, "m_glink");
	}

	//initialises the systems
	void startGLinking(GraphicsState* gState, Registar* registar) {
		auto sGraphics = createSystem<SystemGraphics>(registar);
		sGraphics->setGraphicsState(gState);
		sGraphics->setCamera(gState->getCamera());

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
};


#endif