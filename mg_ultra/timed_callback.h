/*Allows a function to be called against
a given parameter
Utilises win32 multimedia timers*/
#ifndef __TIMED_CALLBACK__
#define __TIMED_CALLBACK__

#include <Windows.h>
#include <functional>

template <class T>
struct TimedCallbackPack {
	function<void(T)> callback;
	T parameter;

	TimedCallbackPack(std::function<void(T)> callback, T parameter)
		: parameter(parameter) {
		this->callback = callback;
	}
};

/*A class that wraps a multimedia timer*/
template <class T>
class TimedCallback {
	HANDLE multimediaTimer;
	int msPeriod;
	TimedCallbackPack<T> callbackPack;

	/*The actual callback used by system*/
	template <class T>
	static void CALLBACK hardwareCallback(PVOID packPointer, BOOLEAN TimerOrWaitFired) {
		TimedCallbackPack<T>* pack = (TimedCallbackPack<T>*)packPointer;
		pack->callback(pack->parameter);
	}

public:
	/*Creates a new timer that will evecute every msPeriod
	Takes a parameter of type T
	And a callbackfunction that will execute against the parameter*/
	TimedCallback(int msPeriod, std::function<void(T)> callbackFunction, T parameter)
		: callbackPack(callbackFunction, parameter) {
		this->msPeriod = msPeriod;
	}

	/*Begins the timer*/
	void startTimer(int delay = 10) {
		CreateTimerQueueTimer(&multimediaTimer, NULL, &hardwareCallback<T>, &callbackPack, delay, msPeriod, WT_EXECUTEINTIMERTHREAD);
	}

	/*Stops the timer*/
	void stopTimer() {
		DeleteTimerQueueTimer(NULL, multimediaTimer, INVALID_HANDLE_VALUE);
	}

	/*Returns a reference to Timer's parametes*/
	T& getParameter() {
		return callbackPack.parameter;
	}
};

#endif