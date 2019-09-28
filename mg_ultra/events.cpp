#include "events.h"

#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>

queue<Event*> eventQueue;
atomic<bool> pipelineOpen = true;
mutex lck;
condition_variable gate;

void g_events::pushEvent(Event* event) {
	unique_lock<mutex> l(lck);
	if (!pipelineOpen) {
		delete event;
		return;
	}
	eventQueue.push(event);
	l.unlock();
	gate.notify_one();
}

bool g_events::pollEvents(Event** event) {
	//wait for event to come in
	unique_lock<mutex> l(lck);
	gate.wait(l, [] { return g_events::queueSize(); });

	Event* currentEvent = eventQueue.front();
	*event = currentEvent;
	eventQueue.pop();
	return true;
}

int g_events::queueSize() {
	return (int)eventQueue.size();
}

void g_events::closeEventPipeline() {
	unique_lock<mutex> l(lck);
	pipelineOpen = false;
}