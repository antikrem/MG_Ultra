#include "events.h"

#include <queue> 
#include <mutex>

queue<Event> eventQueue;
mutex lck;

void g_events::pushEvent(Event event) {
	unique_lock<mutex> l(lck);
	eventQueue.push(event);
}

bool g_events::pollEvents(Event* event) {
	unique_lock<mutex> l(lck);
	if (!eventQueue.size()) {
		return false;
	}

	*event = eventQueue.front();
	eventQueue.pop();
	return true;
}

int g_events::queueSize() {
	return (int)eventQueue.size();
}