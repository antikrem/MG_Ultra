#include "point_light.h"

#include <mutex>
#include <cmath>

#define PERLIN_TEXTURE_WIDTH 1024.0f

mutex gPointLightLock;
Point2 combinedDrift = Point2(0.0f);

void g_pointlights::updateDrift(const Point2& shift) {
	unique_lock<mutex> lck(gPointLightLock);
	combinedDrift = Point2(
		fmod(combinedDrift.x + shift.x, PERLIN_TEXTURE_WIDTH),
		fmod(combinedDrift.y + shift.y, PERLIN_TEXTURE_WIDTH)
	);
}


Point2 g_pointlights::getDrift() {
	unique_lock<mutex> lck(gPointLightLock);
	return combinedDrift;
}
