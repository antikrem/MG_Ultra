/*Allows timed event calling*/
#ifndef __TIMED_EVENT_CALLBACK__
#define __TIMED_EVENT_CALLBACK__

#include "events.h"

#include "constants.h"
#include "timed_callback.h"

/*pushes an event on timer*/
class TimedEventCallback {
	TimedCallback<Event> internalTimer;
	
public:
	TimedEventCallback(int callsPerSecond, string systemName, bool pause) 
		: internalTimer(callsPerSecond, &emitEvent, Event(EV_invokeSystemMaster))  {
		internalTimer.getParameter().data.push_back(systemName);
		internalTimer.getParameter().data.push_back("timer");
		internalTimer.getParameter().data.push_back(pause ? "p" : "np");
	}

	static void emitEvent(Event event) {
		g_events::pushEvent(new Event(&event));
	}

	void startTimer() {
		internalTimer.startTimer();
	}

	void stopTimer() {
		internalTimer.pauseTimer();
	}

};


#endif