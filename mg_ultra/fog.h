/*interface for fog*/
#ifndef __FOG__
#define __FOG__

#include "cus_struct2.h"
#include <tuple>

namespace g_fog {
	//sets fog colour
	void setFogColour(const Point3& colour);

	//sets fog colour
	void l_setFogColour(float r, float g, float b);

	//gets fog colour
	Point3 getFogColour();

	//gets fog colour
	tuple<float, float, float> l_getFogColour();

	//set atmosphere fog scattering
	void setFogStrength(float fogStrength);

	//set atmosphere fog scattering
	float getFogStrength();

	//set fog start
	void setFogStart(float distance);

	//get fog start
	float getFogStart();

	//set fog clamp
	void setFogMax(float fogMax);

	//get fog strength
	float getFogMax();
}

#endif