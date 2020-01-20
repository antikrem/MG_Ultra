#include "fog.h"

#include <mutex>

//access lock for fog values
mutex fogLock;

//colour of fog
Point3 fogColour(0.0f);

float fogStrength = 0;

float fogStart = 0;

float fogMax = 1.0f;

void g_fog::setFogColour(const Point3& colour) {
	unique_lock<mutex> lck(fogLock);
	fogColour = colour;
}

void g_fog::l_setFogColour(float r, float g, float b) {
	g_fog::setFogColour(Point3(r, g, b));
}

Point3 g_fog::getFogColour() {
	unique_lock<mutex> lck(fogLock);
	return fogColour;
}

tuple<float, float, float> g_fog::l_getFogColour() {
	Point3 result = getFogColour();
	return make_tuple(result.x, result.y, result.z);
}

void g_fog::setFogStrength(float strength) {
	unique_lock<mutex> lck(fogLock);
	fogStrength = strength;
}

float g_fog::getFogStrength() {
	unique_lock<mutex> lck(fogLock);
	return fogStrength;
}

void g_fog::setFogStart(float distance) {
	unique_lock<mutex> lck(fogLock);
	fogStart = distance;
}

float g_fog::getFogStart() {
	unique_lock<mutex> lck(fogLock);
	return fogStart;
}

void g_fog::setFogMax(float cap) {
	unique_lock<mutex> lck(fogLock);
	fogMax = cap;
}

float g_fog::getFogMax() {
	unique_lock<mutex> lck(fogLock);
	return fogMax;
}