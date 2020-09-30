/**
 * Allows a function to be called against
 * a given parameter
 * Utilises win32 multimedia timers
 */
#ifndef __TIMED_CALLBACK__
#define __TIMED_CALLBACK__

#include <functional>

#include <iostream>
#include <chrono>
#include <atomic>
#include <mutex>

// Executor callback instance, keepts a pointer to required function
// And void parameter to invoke
class CallbackExecutor {
	std::mutex periodLock;
	std::chrono::microseconds period;
	std::chrono::microseconds passed = std::chrono::microseconds(0);

	std::atomic<bool> active = false;
	std::atomic<bool> invalid = false;

	std::function<void(void*)> callback;
	void* parameter;

public:
	CallbackExecutor(std::chrono::microseconds period, std::function<void(void*)> callback, void* parameter) {
		this->period = period;
		this->callback = callback;
		this->parameter = parameter;
	}

	bool isValid() {
		return !invalid;
	}

	void invalidate() {
		active = false;
		invalid = true;
	}

	void update(std::chrono::microseconds elapsed) {
		if (!active) {
			return;
		}

		passed += elapsed;

		std::unique_lock<std::mutex>(lock);
		if (passed > period) {
			callback(parameter);
			passed -= period;
		}
	}

	void start(std::chrono::microseconds delay = std::chrono::microseconds(0)) {
		passed = -delay;
		active = true;
	}

	void pause() {
		active = false;
	}

	void changePeriod(std::chrono::microseconds period, std::chrono::microseconds delay = std::chrono::microseconds(0)) {
		std::unique_lock<std::mutex>(lock);

		this->period = period;
		passed -= delay;
	}
};

template <class T>
struct TimedCallbackPack {
	std::function<void(T)> callback;
	T parameter;

	TimedCallbackPack(std::function<void(T)> callback, T parameter)
		: parameter(parameter) {
		this->callback = callback;
	}

};

namespace g_timer {
	// Initialiser


	// Periodic callback required to update callbacks
	void updateTimers();

	// Registers a new callback executor
	CallbackExecutor* newTimer(std::chrono::microseconds period, std::function<void(void*)> callback, void* parameter);
}

/**
 * A class that wraps a multimedia timer
 */
template <class T>
class TimedCallback {
	TimedCallbackPack<T> callbackPack;

	// Pointer to underlying executor that is managed by g_timers
	CallbackExecutor* executor;

	/**
	 * The actual callback used by system
	 */
	template <class T>
	static void underlyingCallback(void* packPointer) {
		TimedCallbackPack<T>* pack = (TimedCallbackPack<T>*)packPointer;
		pack->callback(pack->parameter);
	}

public:
	/**
	 * Creates a new timer that will evecute every msPeriod
	 * Takes a parameter of type T
	 * And a callbackfunction that will execute against the parameter
	 */
	TimedCallback(std::chrono::microseconds period, std::function<void(T)> callbackFunction, T parameter)
		: callbackPack(callbackFunction, parameter) {

		// Create a timer within g_timer
		executor = g_timer::newTimer(period, underlyingCallback<T>, &callbackPack);
	}

	/**
	 * Creates a new timer that will evecute given number of times per second
	 * Takes a parameter of type T
	 * And a callbackfunction that will execute against the parameter
	 */
	TimedCallback(int callsPerSecond, std::function<void(T)> callbackFunction, T parameter)
		: callbackPack(callbackFunction, parameter) {

		std::chrono::microseconds period = std::chrono::microseconds(1000000 / callsPerSecond);

		// Create a timer within g_timer
		executor = g_timer::newTimer(period, underlyingCallback<T>, &callbackPack);
	}

	~TimedCallback() {
		executor->invalidate();
	}


	/**
	 * Begins the timer
	 */
	void startTimer(std::chrono::microseconds delay = std::chrono::microseconds(0)) {
		executor->start(delay);
	}

	/**
	 * Pause the timer
	 */
	void pauseTimer() {
		executor->pause();
	}

	/**
	 * Sets a new period
	 */
	void changePeriod(std::chrono::microseconds period, std::chrono::microseconds delay = std::chrono::microseconds(0)) {
		executor->changePeriod(period, delay);
	}

	/**
	 * Returns a reference to Timer's parametes
	 */
	T& getParameter() {
		return callbackPack.parameter;
	}
};


#endif