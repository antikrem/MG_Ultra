#ifndef __ENTITY_TYPES__
#define __ENTITY_TYPES__

using namespace std;

#include <string>

enum EntityTypes {
	ETNoType,
	ETGeneric,
	ETConsole,
	ETPlayer
};

//String to Entity type
//returns -1 on failure
int stringToEntType(string name);

#endif
