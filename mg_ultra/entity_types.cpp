#include "entity_types.h"

#include <map>

const map<string, int> lookup = {
	{"ETNoType", (int)ETNoType},
	{"NoType", (int)ETNoType},
	{"ETGeneric", (int)ETGeneric},
	{"Generic", (int)ETGeneric}
};

//resolve string to ent type
int stringToEntType(const string str) {
	if (lookup.count(str)) {
		return lookup.at(str);
	}
	return -1;
	
}