#include "events.h"

#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>

queue<Event*> eventQueue;
atomic<bool> pipelineOpen = true;
mutex lck;

void g_events::pushEvent(Event* event) {
	unique_lock<mutex> l(lck);
	if (!pipelineOpen) {
		delete event;
		return;
	}
	eventQueue.push(event);
	l.unlock();
}

bool g_events::pollEvents(Event** event) {
	//wait for event to come in
	unique_lock<mutex> l(lck);

	if (eventQueue.size() > 0) {
		Event* currentEvent = eventQueue.front();
		*event = currentEvent;
		eventQueue.pop();
		return true;
	}
	return false;
}

void g_events::clearEventQueue() {
	unique_lock<mutex> l(lck);
	//cannot delete starters, otherwise some
	//systems will stop
	vector<Event*> starters;
	while (!eventQueue.empty()) {
		Event* event = eventQueue.front();
		if (event->data[1] == "starter") {
			starters.push_back(event);
		}
		else {
			delete event;
		}
		eventQueue.pop();
	}
	//add back starters
	for (auto i : starters) {
		eventQueue.push(i);
	}
}

int g_events::queueSize() {
	return (int)eventQueue.size();
}

void g_events::closeEventPipeline() {
	unique_lock<mutex> l(lck);
	pipelineOpen = false;
}