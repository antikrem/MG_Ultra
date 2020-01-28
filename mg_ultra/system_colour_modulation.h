#ifndef __SYSTEM_COLOUR_MODULATION__
#define __SYSTEM_COLOUR_MODULATION__

#include "system.h"

#include "colour_modulation.h"

class SystemColourModulation : public System {

public:
	SystemColourModulation() {
		debugName = "s_colour_modulation";
		//empty system
		target = SubPoolTarget();
	}

	void precycle(EntityPool* pool) override {
		g_colour_modulation::update();
	}

};

#endif