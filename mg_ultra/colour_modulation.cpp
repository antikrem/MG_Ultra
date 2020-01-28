#include "colour_modulation.h"

ColourModulation colourModulation;

void g_colour_modulation::update() {
	colourModulation.update();
}

void g_colour_modulation::setStrengths(float current, float rate, float target) {
	colourModulation.setStrengths(current, rate, target);
}

float g_colour_modulation::getForegroundStrength() {
	return colourModulation.getForegroundModulatingStrength();
}

float g_colour_modulation::getBackgroundStrength() {
	return colourModulation.getBackgroundModulatingStrength();
}

void g_colour_modulation::setForegroundColour(Point3 colour) {
	colourModulation.setForegroundColour(colour);
}

void g_colour_modulation::l_setForegroundColour(float x, float y, float z) {
	Point3 colour(x, y, z);
	colourModulation.setForegroundColour(colour);
}

Point3 g_colour_modulation::getForegroundColour() {
	return colourModulation.getForegroundColour();
}

tuple<float, float, float> g_colour_modulation::l_getForegroundColour() {
	Point3 colour = colourModulation.getForegroundColour();
	return make_tuple(colour.x, colour.y, colour.z);
}

void g_colour_modulation::setBackgroundColour(Point3 colour) {
	colourModulation.setBackgroundColour(colour);
}

void g_colour_modulation::l_setBackgroundColour(float x, float y, float z) {
	Point3 colour(x, y, z);
	colourModulation.setBackgroundColour(colour);
}

Point3 g_colour_modulation::getBackgroundColour() {
	return colourModulation.getBackgroundColour();
}

tuple<float, float, float> g_colour_modulation::l_getBackgroundColour() {
	Point3 colour = colourModulation.getBackgroundColour();
	return make_tuple(colour.x, colour.y, colour.z);
}

void g_colour_modulation::setForegroundCutoff(float cutoff) {
	colourModulation.setForegroundCutoff(cutoff);
}

float g_colour_modulation::getForegroundCutoff() {
	return colourModulation.getForegroundCutoff();
}

void g_colour_modulation::setBackgroundCutoff(float cutoff) {
	colourModulation.setBackgroundCutoff(cutoff);
}

float g_colour_modulation::getBackgroundCutoff() {
	return colourModulation.getBackgroundCutoff();
}