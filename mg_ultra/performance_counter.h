/*Allows profiling of counts
Mostly for guaging performance of a
looping cycle*/
#ifndef __PERFORMANCE_COUNTER__
#define __PERFORMANCE_COUNTER__

#include <chrono>
#include <atomic>

#include "constants.h"

/*Thread safe on multiple queries*/
class PerformanceCounter {
	int count = 0;
	chrono::system_clock::time_point lastReportTime;
	atomic<float> lastReportCount;

	inline void updateReport(long long duration) {
		lastReportCount = (MS_IN_SECOND * (float)count) / duration;
		count = 0;
		lastReportTime = chrono::system_clock::now();
	}

public:
	PerformanceCounter() {
		lastReportTime = chrono::system_clock::now();
		lastReportCount = 0;
	}

	//increments a count
	void increment() {
		count++;
		long long duration = chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now() - lastReportTime).count();
		if (duration > MS_IN_SECOND) {
			updateReport(duration);
		}
	}

	//gets the number of calls last second
	float getReportedCalls() {
		return lastReportCount;
	}
};

#endif