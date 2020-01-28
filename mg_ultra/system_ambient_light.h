#ifndef __SYSTEM_AMBIENT_LIGHT__
#define __SYSTEM_AMBIENT_LIGHT__

#include "system.h"

#include "ambient_illumination.h"

class SystemAmbientLighting : public System {

public:
	SystemAmbientLighting() {
		debugName = "s_ambient_lighting";
		//empty system
		target = SubPoolTarget();
	}

	void precycle(EntityPool* pool) override {
		g_ambient::update();
	}

};

#endif