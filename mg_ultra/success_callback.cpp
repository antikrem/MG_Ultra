#include "success_callback.h"

bool SuccessCallback::waitForCompletion() {
	unique_lock<std::mutex> lk(lock);
	cv.wait(lk, [this] {return this->complete.load(); });
	return result;
}