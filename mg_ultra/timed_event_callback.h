/*Allows timed event calling*/
#ifndef __TIMED_EVENT_CALLBACK__
#define __TIMED_EVENT_CALLBACK__

#include "events.h"
#include "graphics_state.h"

/*pushes an event on timer*/
class TimedEventCallback {
	HANDLE multimediaTimer;
	int msPeriod;
	Event event;
	
public:
	TimedEventCallback(int callsPerSecond, string systemName) : event(EV_invokeSystemMaster)  {
		this->msPeriod = MS_IN_SECOND / callsPerSecond;
		this->event.data.push_back(systemName);
		this->event.data.push_back("timer");
	}

	//can be used for more complex events, that arent a systems invoke
	TimedEventCallback(int callsPerSecond, EventType eventType) : event(eventType) {
		this->msPeriod = MS_IN_SECOND / callsPerSecond;
	}

	static void CALLBACK emitEvent(PVOID event, BOOLEAN TimerOrWaitFired) {
		g_events::pushEvent(new Event((Event*)event));
	}

	void startTimer() {
		CreateTimerQueueTimer(&multimediaTimer, NULL, &emitEvent, &event, 10, msPeriod, WT_EXECUTEINTIMERTHREAD);
	}

	void stopTimer() {
		DeleteTimerQueueTimer(NULL, multimediaTimer, INVALID_HANDLE_VALUE);
	}

};


#endif