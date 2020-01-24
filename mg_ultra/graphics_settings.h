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
	atomic<int> depthPeelingPasses = 4;

	//exposure for the scene
	atomic<float> exposure = 0.3f;
	//bloom threshold
	atomic<float> bloomThreshold = 1.0f;
	//bloom deviation
	atomic<float> bloomDeviation = 5.0f;

	//rendering constants
	atomic<int> countMaxSpriteBox = 1000;
	atomic<int> countMaxUIBox = 1000;
	atomic<int> countMaxDirectionalLights = 10;
	atomic<int> countMaxPointLights = 100;

	//lighting constants
	atomic<int> pointLightPeels = 2;

	//target fps
	atomic<int> targetFPS = 0;

	GraphicsSettings();
};

namespace g_graphicsSettings {
	void setGlobalGraphicsSettings(GraphicsSettings* globalGraphicsSettings);

	//updates some parameters of graphics settings
	void update();

	//sets target
	void setExposureTarget(float target);

	//sets rate
	void setExposureRate(float rate);

	//allows exposure to be directly set
	void setExposure(float exposure);
	
	//sets threshold for bloom
	void setBloomThreshold(float threshold);

	//sets deviation for bloom
	void setBloomDeviation(float deviation);
}

#endif
