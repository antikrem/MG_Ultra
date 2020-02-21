/*A directional light unformly lights
from a given direction*/
#ifndef __POINT_LIGHT__
#define __POINT_LIGHT__

#include "cus_struct2.h"
#include "math_ex.h"
#include "trending_value.h"

#include <atomic>
#include <mutex>

#define DEFAULT_LIGHT_PEAK 0.4f
#define DEFAULT_LIGHT_CUTOFF 0.01f
#define DEFAULT_LIGHT_CUTDISTANCE 270.0f

#define POINT_LIGHT_EXTINCTION_LEVEL 0.0045f

#define POINT_LIGHT_ARBITRARY_CUTOFF 0.00000001

//global point light fog drift variable
namespace g_pointlights {
	//updates drift against noise width
	void updateDrift(const Point2& shift);

	//gets the drift
	Point2 getDrift();
}

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

	float strength;

	//default constructor has uninitialised values
	PointLightData() {

	}


	PointLightData(const Point3& position, const Point3& colour, float a, float b, float c, float extinctionRange, float strength) {
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

		this->strength = strength;
	}
};

class PointLight {
private:
	atomic<Point3> position;
	atomic<Point3> colour;

	//values for generating 
	atomic<float> a = 0.0f;
	atomic<float> b = 0.0f;
	atomic<float> c = (2.0f / POINT_LIGHT_EXTINCTION_LEVEL); //Choose c s.t. its always less than extinction 

	atomic<float> extinctionRange;

	TrendingValue<float> strength = 1.0f;

	void updateExtinctionRange() {
		auto solutions = math_ex::solve_quadratic(
			POINT_LIGHT_EXTINCTION_LEVEL * a.load(),
			POINT_LIGHT_EXTINCTION_LEVEL * b.load(),
			POINT_LIGHT_EXTINCTION_LEVEL * c.load(),
			strength.get()
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
			extinctionRange,
			strength.get()
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

	void setStrength(float strength) {
		this->strength.set(strength);
		updateExtinctionRange();
	}

	void setStrength(float target, float rate) {
		strength.set(strength.get(), rate, target);
		updateExtinctionRange();
	}

	void update() {
		float change = abs(strength.updateAndGetChange());
		if (change > POINT_LIGHT_ARBITRARY_CUTOFF) {
			updateExtinctionRange();
		}
	}
};

#endif