/*Allows callbacks to be assigned to entities on a timer*/
#ifndef __COMPONENT_TIMED_CALLBACK__
#define __COMPONENT_TIMED_CALLBACK__

#include "component.h"
#include "constants.h"
#include "mmap.h"
#include "script_master.h"

#include <vector>
#include <shared_mutex>

class ComponentTimedCallback : public Component {
	mutex lock;
	MMap<int, string> timedCallbacks;

public:
	ComponentTimedCallback() {
		timedCallbacks.clear();
	}

	ComponentTimedCallback(int cycle, string callback) {
		timedCallbacks.clear();
	}

	void addCallback(int cycle, string callback) {
		lock_guard<mutex> lck(lock);
		timedCallbacks[cycle].push_back(callback);
	}

	//get vector of all strings
	vector<string> getCallbacks(int cycle, shared_ptr<Entity> ptr) {
		if (timedCallbacks.count(cycle)) {
			return timedCallbacks[cycle];
		}

		return vector<string>();
	}

};



#endif