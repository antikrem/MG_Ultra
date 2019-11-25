#ifndef __GRAPHICS_SETTINGS__
#define __GRAPHICS_SETTINGS__

#include <mutex>

using namespace std;

//A struct that contains all the settings for graphics
struct GraphicsSettings {
	//Access lock
	mutex accessLock;
	//level of antialiasing used, must be in the form 2^n or 0
	int antialiasing = 4;
	//Size of window
	int screenWidth = 1366, screenHeight = 768;
};

#endif
