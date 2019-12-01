/*Contains information for illumination globally
Which affects all entities uniformly*/
#ifndef __AMBIENT_ILLUMINATION__
#define __AMBIENT_ILLUMINATION__

#include <atomic>
#include <mutex>
#include <tuple>
#include "cus_struct2.h"

/*Provides all information on global ambient illumination*/
class AmbientIllumination {
	//access lock for color
	mutex colourLock;
	//colour of ambient lighting
	Point3 colour;
	//portion of lighting produced by ambient
	atomic<float> strength = 1.0f;

public:
	AmbientIllumination()
		: colour(1.0f, 1.0f, 1.0f) {

	}

	void setStrength(float strength) {
		this->strength = strength;
	}

	float getStrength() {
		return strength;
	}

	void setColour(const Point3& colour) {
		unique_lock<mutex> lck(colourLock);
		this->colour = colour;
	}
	
	Point3 getColour() {
		unique_lock<mutex> lck(colourLock);
		return colour;
	}
};

/*global accessor for ambient values*/
namespace g_ambient {
	//negative values are undefined behaviour
	void setStrength(float value);

	float getStrength();

	void setColour(const Point3& colour);

	Point3 getColour();

	void l_setColour(float colourX, float colourY, float colourZ);

	tuple<float, float, float> l_getColour();
}

#endif
