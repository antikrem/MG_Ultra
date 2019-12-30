//Encapsulates a single applier of force
//most particle information is tied to external classes
#ifndef __FORCE_APPLIER__
#define __FORCE_APPLIER__

#include <atomic>
#include "algorithm_ex.h"

#include "cus_struct2.h"

#define DEFAULT_PEAK 0.01f

//Minimal specification required for a force applier
struct ForceSpecification {
	Point3 pos;
	
	//distance dropoff equation relative to distance -> d
	//1 / (f * x^2 + p)
	float p;
	float f;


	ForceSpecification(const Point3& position, float p, float f)
	: pos(position) {
		this->p = p;
		this->f = f;
	}
};

struct ForceApplier {
	atomic<Point3> position;

	atomic<float> cutoff = DEFAULT_PEAK;
	atomic<float> cutdistance = DEFAULT_PEAK;
	atomic<float> peak = DEFAULT_PEAK;

	//an uninitialised Force applier
	ForceApplier()
		: position(0.0f) {
	}

	ForceSpecification getSpecification() {
		return ForceSpecification(
			position.load(),
			//solve for p
			1.0f / peak,
			//solve of slope factor f
			(1.0f - cutoff) / (peak * cutoff * pow(cutdistance, 2) )
		);
	}
};

#endif