/*Timer object
Allows time to be measured in micro seconds*/
#ifndef __TIMER__
#define __TIMER__

#include <chrono>

using namespace std;

/*Used to measure how much time has passed since constructor
Thread safe*/
class Timer {
	//Starting chrono time
	chrono::high_resolution_clock::time_point start;

public:
	/*Time start is set on constructor*/
	Timer();

	/*Time returned is in microseconds*/
	unsigned long long int getTime();
};


#endif