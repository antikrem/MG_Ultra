/*Encapsulates a listener of audio
*/
#ifndef __AUDIO_LISTENER__
#define __AUDIO_LISTENER__

#include "audio_file.h"
#include "cus_struct2.h"

#include <mutex>


class AudioListener {
	mutex lock;
	bool listenTargetUpdated = false;
	Point3 listenTarget = Point3(0, 1, 0);

public:
	void setTarget(const Point3& position) {
		unique_lock<mutex> lck(lock);
		listenTargetUpdated = false;
		this->listenTarget = position;
	}

	Point3 getTarget() {
		unique_lock<mutex> lck(lock);
		return listenTarget;
	}

	//returns true if an udpate is required
	bool listenTargetUpdateRequired() {
		return XOR(listenTargetUpdated, (listenTargetUpdated = true));
	}
};


#endif