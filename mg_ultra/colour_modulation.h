/*Allows modulation of colours based on level depth*/
#ifndef __COLOUR_MODULATION__
#define __COLOUR_MODULATION__

#include <atomic>
#include <mutex>
#include <tuple>

#include "cus_struct2.h"

#include "trending_value.h"

/*Provides all information on global ambient illumination*/
class ColourModulation {
	//foreground modulating colour
	atomic<Point3> foregroundModulatingColour;

	//foreground modulating strength
	TrendingValue<float> foregroundModulatingStrength;

	//foreground ending cutoff
	atomic<float> foregroundCutoff = -0.1f;

	//background modulating colour
	atomic<Point3> backgroundModulatingColour;

	//background modulating strength
	TrendingValue<float> backgroundModulatingStrength;

	//background ending cutoff
	atomic<float> backgroundCutoff = 0.1f;


public:
	ColourModulation()
		: foregroundModulatingColour(0.0f)
		, foregroundModulatingStrength(0.0f)
		, backgroundModulatingColour(0.0f)
		, backgroundModulatingStrength(0.0f)  {

	}

	//updates strength tending values
	void update() {
		foregroundModulatingStrength.update();
		backgroundModulatingStrength.update();
	}

	//set values for both mod strengths
	void setStrengths(float current, float rate, float target) {
		backgroundModulatingStrength.setCurrent(current);
		backgroundModulatingStrength.setRate(rate);
		backgroundModulatingStrength.setTarget(target);

		foregroundModulatingStrength.setCurrent(current);
		foregroundModulatingStrength.setRate(rate);
		foregroundModulatingStrength.setTarget(target);
	}

	//get foreground modulating strength
	float getForegroundModulatingStrength() {
		return foregroundModulatingStrength.get();
	}

	//get background modulating strength
	float getBackgroundModulatingStrength() {
		return backgroundModulatingStrength.get();
	}

	//set foreground colour
	void setForegroundColour(Point3 foregroundModulatingColour) {
		this->foregroundModulatingColour = foregroundModulatingColour;
	}

	//get foreground colour
	Point3 getForegroundColour() {
		return foregroundModulatingColour;
	}

	//set background colour
	void setBackgroundColour(Point3 backgroundModulatingColour) {
		this->backgroundModulatingColour = backgroundModulatingColour;
	}

	//get foreground colour
	Point3 getBackgroundColour() {
		return backgroundModulatingColour;
	}

	//sets foreground cutoff
	void setForegroundCutoff(float foregroundCutoff) {
		this->foregroundCutoff = foregroundCutoff;
	}

	//gets foreground cutoff
	float getForegroundCutoff() {
		//need to garuntee foreground < background
		return min(foregroundCutoff.load(), backgroundCutoff - 1.0f);
	}
	
	//sets backgroundground cutoff
	void setBackgroundCutoff(float backgroundCutoff) {
		this->backgroundCutoff = backgroundCutoff;
	}

	//gets background cutoff
	float getBackgroundCutoff() {
		return backgroundCutoff;
	}
};

/*global accessor for ambient values*/
namespace g_colour_modulation {
	//update modulation
	void update();

	//set strength values for both layers
	void setStrengths(float current, float rate, float target);

	//get foreground strength
	float getForegroundStrength();

	//get background strength
	float getBackgroundStrength();

	//sets foreground colour
	void setForegroundColour(Point3 colour);

	void l_setForegroundColour(float x, float y, float z);

	//gets foreground colour
	Point3 getForegroundColour();

	tuple<float, float, float> l_getForegroundColour();

	//sets background colour
	void setBackgroundColour(Point3 colour);

	void l_setBackgroundColour(float x, float y, float z);

	//gets background colour
	Point3 getBackgroundColour();

	tuple<float, float, float> l_getBackgroundColour();

	//sets cutoff for foreground
	void setForegroundCutoff(float cutoff);

	//gets cutoff for foreground
	float getForegroundCutoff();
	
	//set cutoff for background
	void setBackgroundCutoff(float cutoff);

	//get cutoff for background
	float getBackgroundCutoff();
}

#endif