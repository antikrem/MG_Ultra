#ifndef __GRAPHICS_SETTINGS__
#define __GRAPHICS_SETTINGS__

#include <mutex>
#include <atomic>

using namespace std;

//A struct that contains all the settings for graphics
struct GraphicsSettings {
	//level of antialiasing used, must be in the form 2^n or 0
	atomic<int> antialiasing = 4;
	//Size of window
	atomic<int> screenWidth = 1366, screenHeight = 768;
	//number of passes for depth peeling
	atomic<int> depthPeelingPasses = 2;
};

#endif
