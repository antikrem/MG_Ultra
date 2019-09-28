/*Allows asyncronous tasks to be waited on until completion*/

#include <atomic>
#include <condition_variable>

using namespace std;

#ifndef __SUCCESS_CALLBACK__
#define __SUCCESS_CALLBACK__

class SuccessCallback {
	//lock for cv
	mutex lock;
	condition_variable cv;

	atomic<bool> complete = false;
	bool result;

public:

	void setCompletion(bool successful) {
		{
			lock_guard<std::mutex> lk(lock);
			result = successful;
			complete = true;
		}
		cv.notify_one();
	}

	bool waitForCompletion();

	void reset() {
		lock_guard<std::mutex> lk(lock);
		complete = false;
	}

};

#endif