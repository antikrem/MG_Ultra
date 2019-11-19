/*Can block a thread for a defined duration
motivating usage was controlling framerate*/
#ifndef __TIMED_BLOCK__
#define __TIMED_BLOCK__

#include <atomic>
#include <chrono>
#include <thread>

#include "constants.h"

/*Can block a thread to enforce period
Changing time is thread-safe
No guarantee of accuracy*/
class TimedBlock {
	atomic<bool> active;
	atomic<int> frequency;
	chrono::time_point<chrono::steady_clock> point;

public:
	/*Initialise the TimedBlock*/
	TimedBlock(int frequency) {
		point = chrono::steady_clock::now();
		setfrequency(frequency);
	}

	/*sets the frequency of this TimedBlock
	set to value 0 to disable wating*/
	void setfrequency(int frequency) {
		if (frequency) {
			active = true;
			this->frequency = frequency;
		}
		else {
			active = 0;
		}
	}

	/*Blocks to adhere to period*/
	void blockForTime() {
		if (active) {
			this_thread::sleep_until(point + (chrono::microseconds(1000000 / frequency)));
		}
		
		point = chrono::steady_clock::now();
	}


};


#endif