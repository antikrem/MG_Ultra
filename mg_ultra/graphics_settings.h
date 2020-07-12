#ifndef __GRAPHICS_SETTINGS__
#define __GRAPHICS_SETTINGS__

#include <mutex>
#include <atomic>

using namespace std;

// A struct that contains all the settings for graphics
struct GraphicsSettings {
	// Is full screen
	atomic<int> fullScreen = false;

	// Level of antialiasing used, must be in the form 2^n or 0
	atomic<int> antialiasing = 4;
	// Size of window
	atomic<int> screenWidth = 1366, screenHeight = 768;
	// Number of passes for depth peeling
	atomic<int> depthPeelingPasses = 4;

	// Exposure for the scene
	atomic<float> exposure = 0.3f;
	// Bloom threshold
	atomic<float> bloomThreshold = 1.0f;
	// Bloom deviation
	atomic<float> bloomDeviation = 5.0f;

	// Rendering constants
	atomic<int> countMaxSpriteBox = 1000;
	atomic<int> countMaxUIBox = 1000;
	atomic<int> countMaxDirectionalLights = 10;
	atomic<int> countMaxPointLights = 100;

	// Lighting constants
	atomic<int> pointLightVolumetric = true;
	atomic<int> pointLightPeels = 2;
	atomic<float> pointLightExtinctionRange = 0.0045f;

	// Post effects constants
	atomic<int> bloomEnabled = true;
	atomic<int> bloomPasses = 1;
	atomic<float> bloomResolution = 0.5f;

	// Target fps
	atomic<int> targetFPS = 0;

	GraphicsSettings();
};

namespace g_graphicsSettings {
	void setGlobalGraphicsSettings(GraphicsSettings* globalGraphicsSettings);

	// Updates some parameters of graphics settings
	void update();

	// Sets target
	void setExposureTarget(float target);

	// Sets rate
	void setExposureRate(float rate);

	// Allows exposure to be directly set
	void setExposure(float exposure);
	
	// Sets threshold for bloom
	void setBloomThreshold(float threshold);

	// Sets deviation for bloom
	void setBloomDeviation(float deviation);

	// Get point light extinction range
	float getPointLightExtinctionRange();

}

#endif
