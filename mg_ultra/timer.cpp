#include "timer.h"

Timer::Timer() {
	start = chrono::high_resolution_clock::now();
}

unsigned long long int Timer::getTime() {
	auto difference = (chrono::high_resolution_clock::now() - start);
	unsigned long long int value = chrono::duration_cast<chrono::microseconds>(difference).count();
	return value;
}