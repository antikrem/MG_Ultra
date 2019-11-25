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
	chrono::system_clock::time_point lastFrame;
	atomic<float> lastReportCount;
	atomic<float> lastFPS;

	inline void updateReport(long long duration) {
		lastReportCount = (MS_IN_SECOND * (float)count) / duration;
		count = 0;
		lastReportTime = lastFrame;
	}

public:
	PerformanceCounter() {
		lastReportTime = chrono::system_clock::now();
		lastReportCount = 0;
		lastFPS = 0;
	}

	//increments a count
	void increment() {
		count++;
		
		auto tp = chrono::system_clock::now();
		lastFPS = 1.0f / (((float)chrono::duration_cast<std::chrono::milliseconds>(tp - lastFrame).count()) / MS_IN_SECOND);


		lastFrame = tp;
		long long duration = chrono::duration_cast<std::chrono::milliseconds>(lastFrame - lastReportTime).count();
		if (duration > MS_IN_SECOND) {
			updateReport(duration);
		}
	}

	//gets the number of calls last second
	float getReportedCalls() {
		return lastReportCount;
	}

	//returns fps
	float getFPS() {
		return lastFPS;
	}
};

#endif