#include "ambient_illumination.h"

//global ambient illumination
AmbientIllumination gAmbient;

void g_ambient::update() {
	gAmbient.update();
}

void g_ambient::setStrengths(float current, float rate, float target) {
	gAmbient.setStrengths(current, rate, target);
}

float g_ambient::getStrength() {
	return gAmbient.getStrength();
}

void g_ambient::setColour(const Point3& colour) {
	return gAmbient.setColour(colour);
}

Point3 g_ambient::getColour() {
	return gAmbient.getColour();
}

void g_ambient::l_setColour(float colourX, float colourY, float colourZ) {
	g_ambient::setColour(
		Point3(colourX, colourY, colourZ)
	);
}

tuple<float, float, float> g_ambient::l_getColour() {
	Point3 value = g_ambient::getColour();
	return make_tuple(value.x, value.y, value.z);
}