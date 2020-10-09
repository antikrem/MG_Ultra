#ifndef __ATOMIC_QUEUE__
#define __ATOMIC_QUEUE__

#include <queue>
#include <mutex>

/**
 * A queue that has only atomic operations
 */
template <class T>
class AtomicQueue {
	// Set to true when this queue will be designated to no longer take 
	// inputs 
	bool stopFlag = false;

	// Locked when internal queue is updated
	std::mutex lock;

	// Notified upon state update (queue is added to or closed)
	std::condition_variable stateUpdate;

	std::queue<T> internalQueue;

public:
	// Adds a new element and notifies
	// All waiting get
	void add(const T& element) {
		std::unique_lock<mutex>(lock);
		internalQueue.push(element);
		stateUpdate.notify_one();
	}

	// Gets the first element in the queue
	// Waiting if the queue is empty
	// returns false if this 
	bool get(T* element) {
		std::unique_lock<mutex> l(lock);
		// Check for a new element or stop flag
		stateUpdate.wait(
			l, 
			[&]() {
				return stopFlag || internalQueue.size();
			}
		);

		if (stopFlag) {
			return false;
		}
		else {
			*element = internalQueue.front();
			internalQueue.pop();
			return true;
		}
	}

	// Stop this queue
	// This stops all future adds
	// And makes future gets return false
	void stop() {
		std::unique_lock<mutex>(lock);
		stopFlag = true;
		stateUpdate.notify_one();
	}

	// Returns current size of the queue
	int size() {
		std::unique_lock<mutex>(lock);
		return internalQueue.size();
	}

};

#endif