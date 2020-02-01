/*A directional light unformly lights
from a given direction*/
#ifndef __DIRECTIONAL_LIGHT__
#define __DIRECTIONAL_LIGHT__

#include <mutex>
#include <atomic>

#include "cus_struct2.h"
#include "trending_value.h"

struct DirectionalData {
	float direction[3];
	float colour[3];

	DirectionalData() {
	}

	DirectionalData(const Point3& direction, const Point3& colour, float strength) {
		this->direction[0] = direction.x;
		this->direction[1] = direction.y;
		this->direction[2] = -1 * direction.z;

		this->colour[0] = strength * colour.x;
		this->colour[1] = strength * colour.y;
		this->colour[2] = strength * colour.z;
	}
};

class DirectionalLight {
	TrendingValue<float> strength = 1.0f;

	atomic<Point3> direction;

	atomic<Point3> colour;

public:
	DirectionalLight(const Point3& direction, const Point3& colour)
	: direction(direction), colour(colour) {

	}

	void update() {
		strength.update();
	}

	void setStrength(float current, float rate, float target) {
		strength.set(current, rate, target);
	}

	float getStrength() {
		return strength.get();
	}

	void setDirection(const Point3& direction) {
		this->direction = direction;
	}

	Point3 getDirection() {
		return direction;
	}

	void setColour(const Point3& colour) {
		this->colour = colour;
	}

	Point3 getColour() {
		return colour;
	}

	DirectionalData getData() {
		return DirectionalData(direction, colour, strength.get());
	}
};



#endif