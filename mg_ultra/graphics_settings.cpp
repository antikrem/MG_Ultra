#include <shared_mutex>

#include "graphics_settings.h"
#include "ini_parser.h"
#include "math_ex.h"

#define GFX_FILE_LOCATION "settings/gfx_settings.ini"

shared_mutex globalGSettingsPtrLock;
GraphicsSettings* globalGSettings = nullptr;

atomic<float> targetExposure = 1.0f;
atomic<float> rateExposure = 1.0f;

GraphicsSettings::GraphicsSettings() {
	//load settings from file
	INIParser gfxSettings(GFX_FILE_LOCATION);

	targetFPS = gfxSettings.get("general", "target_fps", 0);

	countMaxSpriteBox = gfxSettings.get("max_renderable", "sprites", 5000);
	countMaxUIBox = gfxSettings.get("max_renderable", "ui", 1500);
	countMaxDirectionalLights = gfxSettings.get("max_renderable", "directional_lights", 10);
	countMaxPointLights = gfxSettings.get("max_renderable", "point_lights", 100);

	pointLightPeels = gfxSettings.get("lighting", "pointlight_peeldepth", 2);

	bloomEnabled = gfxSettings.get("post_effects", "bloom_enabled", 1);
	bloomPasses = gfxSettings.get("post_effects", "bloom_passes", 1);

	depthPeelingPasses = gfxSettings.get("renderer", "transparency_levels", 3);

	g_graphicsSettings::setGlobalGraphicsSettings(this);
}

void g_graphicsSettings::setGlobalGraphicsSettings(GraphicsSettings* globalGraphicsSettings) {
	unique_lock<shared_mutex> lck(globalGSettingsPtrLock);
	globalGSettings = globalGraphicsSettings;
}

void g_graphicsSettings::update() {
	shared_lock<shared_mutex> lck(globalGSettingsPtrLock);
	globalGSettings->exposure = math_ex::tend_to(globalGSettings->exposure.load(), rateExposure.load(), targetExposure.load());
}

void g_graphicsSettings::setExposureTarget(float target) {
	targetExposure = target;
}

void g_graphicsSettings::setExposureRate(float rate) {
	rateExposure = rate;
}

void g_graphicsSettings::setExposure(float exposure) {
	targetExposure = exposure;
	shared_lock<shared_mutex> lck(globalGSettingsPtrLock);
	globalGSettings->exposure = exposure;
}

void g_graphicsSettings::setBloomThreshold(float threshold) {
	shared_lock<shared_mutex> lck(globalGSettingsPtrLock);
	globalGSettings->bloomThreshold = threshold;
}

void g_graphicsSettings::setBloomDeviation(float deviation) {
	shared_lock<shared_mutex> lck(globalGSettingsPtrLock);
	globalGSettings->bloomDeviation = deviation;
}