/*Encapsulates a listener of audio
*/
#ifndef __AUDIO_LISTENER__
#define __AUDIO_LISTENER__

#include "audio_file.h"
#include "cus_struct2.h"

#include <mutex>


class AudioListener {
	mutex lock;

	bool updated = false;
	Point3 listenTarget = Point3(0, 0, -1);
	Point3 upTarget = Point3(0, 1, 0);

public:
	void setListenTarget(const Point3& position) {
		unique_lock<mutex> lck(lock);
		updated = false;
		this->listenTarget = position;
	}

	Point3 getListenTarget() {
		unique_lock<mutex> lck(lock);
		return listenTarget;
	}

	void setUpTarget(const Point3& position) {
		unique_lock<mutex> lck(lock);
		updated = false;
		this->upTarget = position;
	}

	Point3 getUpTarget() {
		unique_lock<mutex> lck(lock);
		return upTarget;
	}

	//updates listener
	void updateListener() {
		unique_lock<mutex> lck(lock);
		if (!updated) {
			updated = false;
			float orentation[6] = {
				listenTarget.x, listenTarget.y, listenTarget.z,
				upTarget.x, upTarget.y, upTarget.z
			};
			alListenerfv(AL_ORIENTATION, orentation);
		}
	}

};


#endif