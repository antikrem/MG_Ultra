/*Basic events system
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
	EV_invokeSystemMaster,
	EV_profilingUpdate,
	EV_pause,
	EV_LENGTH
};

/*Encapsulates an event, this can be thrown from anywhere in the engine*/
struct Event {
	EventType type;
	vector<string> data;

	Event(EventType type) {
		this->type = type;
	}

	//create an event by reference
	Event(const Event& event) {
		this->type = event.type;
		this->data = event.data;
	}
	
	//create an event by pointer to another event
	Event(const Event* event) {
		this->type = event->type;
		this->data = event->data;
	}
};

namespace g_events {
	/*Push the event onto the event que*/
	void pushEvent(Event* event);

	//blocks thread until a new event comes in
	//updates event with pointer to new event
	bool pollEvents(Event** event);

	//clears all events currently in the queue
	void clearEventQueue();

	//closes event pipeline
	//then clears all events in queue
	void closeEventPipeline();

	//debugging
	int queueSize();
}



#endif