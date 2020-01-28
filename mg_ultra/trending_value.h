/*A class for values that utilise
a current value, target and some rate of change*/
#ifndef __TRENDING_VALUE__
#define __TRENDING_VALUE__

#include <atomic>

#include "math_ex.h"

template <class T>
class TrendingValue {
	std::atomic<T> current;

	std::atomic<T> target;

	std::atomic<T> rate;


public:
	TrendingValue(T value)
		: current(value), target(value), rate((T)1) {

	}

	TrendingValue(T value, T rate)
		: current(value), target(value), rate(rate) {

	}

	TrendingValue(T current, T rate, T target)
		: current(current), target(target), rate(rate) {

	}

	void update() {
		current = math_ex::tend_to(current.load(), rate.load(), target.load());
	}

	T get() {
		return current;
	}

	void set(T current, T rate, T target) {
		setCurrent(current);
		setRate(rate);
		setTarget(target);
	}

	void setCurrent(T current) {
		this->current = current;
	}

	void setTarget(T target) {
		this->target = target;
	}

	void setRate(T rate) {
		this->rate = rate;
	}

};


#endif