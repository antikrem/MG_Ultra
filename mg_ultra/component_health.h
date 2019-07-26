/*A Component that is simply a health value, that can be access in a safe way*/
#ifndef __COMPONENT_HEALTH__
#define __COMPONENT_HEALTH__

#include <atomic>
#include "component.h"
#include "constants.h"

class ComponentHealth : public Component {
private:
	atomic<int> health = 1;
public:
	ComponentHealth(int startingHealth) {
		health.store(startingHealth);
	}

	void damage(int damage) {
		health -= damage;
		if (health <= 0) {
			killEntity();
		}
	}


};


#endif