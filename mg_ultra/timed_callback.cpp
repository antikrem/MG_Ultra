#include <vector>

#include "timed_callback.h"
#include "algorithm_ex.h"

// Lock for executor modification
std::mutex lock;

bool first = true;

// List of executors
std::vector<CallbackExecutor*> executors;

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

// Last timepoint executed
TimePoint lastUpdate = std::chrono::high_resolution_clock::now();

void cleanUp() {
	erase_sequential_if(
		executors,
		[](CallbackExecutor* executor) {
			return !executor;
		}
	);
}

void g_timer::updateTimers() {
	std::unique_lock<std::mutex>(lock);

	TimePoint prior = lastUpdate;
	lastUpdate = std::chrono::high_resolution_clock::now();

	if (first) {
		first = false;
		return;
	}

	std::chrono::microseconds duration 
		= std::chrono::duration_cast<std::chrono::microseconds>(lastUpdate - prior);

	bool clearFlag = false;

	for (auto& i : executors) {
		if (i->isValid()) {
			i->update(duration);
		}
		else {
			delete i;
			i = nullptr;
			clearFlag = true;
		}
	}

	if (clearFlag) {
		cleanUp();
	}

}

void g_timer::finalise() {
	std::unique_lock<std::mutex>(lock);

	bool clearFlag = false;

	for (auto& i : executors) {
		if (!i->isValid()) {
			delete i;
			i = nullptr;
			clearFlag = true;
		}
	}

	if (clearFlag) {
		cleanUp();
	}
}

CallbackExecutor* g_timer::newTimer(std::chrono::microseconds period, std::function<void(void*)> callback, void* parameter) {
	CallbackExecutor* executor = new CallbackExecutor(period, callback, parameter);
	executors.push_back(executor);

	return executor;
}