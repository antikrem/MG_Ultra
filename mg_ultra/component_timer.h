/*Stores how many in game cycles/counter this has been alive for*/
#ifndef __COMPONENT_TIMER__
#define __COMPONENT_TIMER__

#include "component.h"
#include "constants.h"

#include <atomic>

class ComponentTimer : public Component {
	atomic<int> cycle = -1;

public:
	void updateTimer() {
		cycle++;
	}

	int getCycle() {
		return cycle;
	}
};



#endif