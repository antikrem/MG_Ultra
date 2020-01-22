/*A directional light unformly lights
from a given direction*/
#ifndef __POINT_LIGHT__
#define __POINT_LIGHT__

#include "cus_struct2.h"
#include "math_ex.h"

#include <atomic>
#include <mutex>

#define DEFAULT_LIGHT_PEAK 0.4f
#define DEFAULT_LIGHT_CUTOFF 0.01f
#define DEFAULT_LIGHT_CUTDISTANCE 270.0f

#define POINT_LIGHT_EXTINCTION_LEVEL 0.0045f

//Minimal specification required for a point light
struct PointLightData {
	float position[3];
	float colour[3];

	//values for light strength
	float a;
	float b;
	float c;

	//light extinction distance
	float extinctionRange;

	//default constructor has uninitialised values
	PointLightData() {

	}


	PointLightData(const Point3& position, const Point3& colour, float a, float b, float c, float extinctionRange) {
		this->position[0] = position.x;
		this->position[1] = position.y;
		this->position[2] = position.z;

		this->colour[0] = colour.x;
		this->colour[1] = colour.y;
		this->colour[2] = colour.z;

		this->a = a;
		this->b = b;
		this->c = c;

		this->extinctionRange = extinctionRange;
	}
};

class PointLight {
private:
	atomic<Point3> position;
	atomic<Point3> colour;

	//values for generating 
	atomic<float> a = 0.0022f;
	atomic<float> b = 0.01f;
	atomic<float> c = 4.0f;

	atomic<float> extinctionRange;

	atomic<float> intensity = 1.0f;

	void updateExtinctionRange() {
		auto solutions = math_ex::solve_quadratic(
			POINT_LIGHT_EXTINCTION_LEVEL * a.load(), 
			POINT_LIGHT_EXTINCTION_LEVEL * b.load(),
			POINT_LIGHT_EXTINCTION_LEVEL * c.load(),
			1.0f
		);
		extinctionRange = max(get<0>(solutions), get<1>(solutions));
	}
	

public:

	PointLight(Point3 position)
	: position(position), colour(1.0f) {
		updateExtinctionRange();
	}

	//construction with colour
	PointLight(Point3 position, Point3 colour)
	: position(position), colour(colour) {
		updateExtinctionRange();
	}

	PointLightData getData() {
		return PointLightData(
			position.load(),
			colour.load(),
			a,
			b,
			c,
			extinctionRange
		);
	}

	void setConstants(float a, float b, float c) {
		this->a = a;
		this->b = b;
		this->c = c;
		updateExtinctionRange();
	}

	void setColour(const Point3& colour) {
		this->colour = colour;
	}

	void setPosition(Point3 position) {
		this->position = position;
	}
};

#endif