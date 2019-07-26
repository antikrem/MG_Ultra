/*Basic events system
Not accessible to lua (yet)
push an event and it will be handled by ecs_master and the main thread
set 
*/
#ifndef __EVENTS__
#define __EVENTS__

using namespace std;

#include <vector>

enum EventType {
	EV_noEvent,
	EV_quit,
	EV_loadNextState,
	EV_LENGTH
};

struct Event {
	EventType type;
	vector<string> data;

	Event(EventType type) {
		this->type = type;
	}

};

namespace g_events {

	void pushEvent(Event event);

	//on true, event will be populated with latest event
	bool pollEvents(Event* event);

	//debugging
	int queueSize();
}



#endif