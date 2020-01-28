#ifndef __SYSTEM_FOG__
#define __SYSTEM_FOG__

#include "system.h"

#include "fog.h"

class SystemFog : public System {

public:
	SystemFog() {
		debugName = "s_fog";
		//empty system
		target = SubPoolTarget();
	}

	void precycle(EntityPool* pool) override {
		g_fog::update();
	}

};

#endif