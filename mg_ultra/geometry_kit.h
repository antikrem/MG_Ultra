/*Helper functions for generating geometry*/
#ifndef __GEOMETRY_KIT__
#define __GEOMETRY_KIT__

#include <vector>

#include "constants.h"

using namespace std;

namespace geom_kit { 

	//creates a regular sphere with given indexes
	void populate_sphere(vector<float>& vertices, vector<int>& indices, int stacks, int sectors, float radius);
}

#endif