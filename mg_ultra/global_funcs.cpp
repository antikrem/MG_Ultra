#include "global_funcs.h"

#include <iostream>
#include <atomic>
#include <mutex>

#include <typeindex>

#include "os_kit.h"
#include "events.h"
#include "pool.h"
#include "texture.h"
#include "script_master.h"
#include "gl_handler.h"
#include "fog.h"
#include "ambient_illumination.h"
#include "audio_master.h"
#include "particle_master.h"
#include "graphics_settings.h"
#include "colour_modulation.h"

///print
//For use with print function, holds a buffer sting to print
string buffer;

void print_addString(const string& in) {
	buffer.append(in);
}

void print_addInt(int in) {
	buffer.append(to_string(in));
}

void print_addBool(bool in) {
	string message = in ? "true" : "false";
	buffer.append(message);
}

void print_push() {
	err::logMessage(buffer);
	buffer.clear();
}

///events
//Factory event template which allows events to be constructed
Event* eventConstruction = nullptr;

//converts an int to a EventType
EventType convertIntToEventType(int flag) {
	if (flag < 0 || flag >= EV_LENGTH) {
		return EV_noEvent;
	}
	return (EventType)flag;
}

//sets event type
void setEvent(int type) {
	if (!eventConstruction) {
		eventConstruction = new Event(convertIntToEventType(type));
		eventConstruction->data.clear();
	}
	else {
		//todo report error
	}
	
}

void addEventData(string data) {
	if (eventConstruction) {
		eventConstruction->data.push_back(data);
	}
	else {
		//todo report error
	}
}

void pushEvent() {
	if (eventConstruction) {
		g_events::pushEvent(eventConstruction);
		eventConstruction = nullptr;
	}
	else {
		//todo report error
	}
}

///profiling
mutex profileLock;
ProfileMap profileInfo;

ProfileMap getProfileInfoMap() {
	unique_lock<mutex> lck(profileLock);
	return profileInfo;
}

void setProfileInfoMap(ProfileMap& profileMap) {
	unique_lock<mutex> lck(profileLock);
	profileInfo = profileMap;
}


void registerGlobalFunctions(kaguya::State &kaguya) {
	//printing
	kaguya["printAdd"] = kaguya::overload(print_addString, print_addInt, print_addBool);
	kaguya["printPush"] = print_push;

	//events
	kaguya["setEvent"] = setEvent;
	kaguya["addEventData"] = addEventData;
	kaguya["pushEvent"] = pushEvent;
	kaguya["get_event_length"] = g_events::queueSize;

	//profiling
	kaguya["getProfileInfo"] = getProfileInfoMap;
	kaguya["getScriptMasterLastCalls"] = g_script::callsLastSecond;
	kaguya["getScriptMasterLastFPS"] = g_script::FPS;
	kaguya["getGLHandlerLastCalls"] = gl_query::getLastCalls;
	kaguya["getGLHandlerLastFPS"] = gl_query::getFPS;
	kaguya["getVMemUsed"] = os_kit::getVMemUsed;

	//animation master
	kaguya["queryAnimation"] = g_aniquery::getAnimationSize;
	
	//fog
	kaguya["setFogColour"] = g_fog::l_setFogColour;
	kaguya["getFogColour"] = g_fog::l_getFogColour;
	kaguya["setFogStrength"] = g_fog::setFogStrength;
	kaguya["getFogStrength"] = g_fog::getFogStrength;
	kaguya["setFogStart"] = g_fog::setFogStart;
	kaguya["getFogStart"] = g_fog::getFogStart;
	kaguya["setFogMax"] = g_fog::setFogMax;
	kaguya["getFogMax"] = g_fog::getFogMax;

	//ambient lighting
	kaguya["setAmbientLightStrengths"] = g_ambient::setStrengths;
	kaguya["getAmbientLightStrength"] = g_ambient::getStrength;
	kaguya["setAmbientLightColour"] = g_ambient::l_setColour;
	kaguya["getAmbientLightColour"] = g_ambient::l_getColour;

	//audio
	kaguya["addAudioFile"] = g_audio::addAudioFile;
	kaguya["printAudioReport"] = g_audio::print;
	kaguya["flushAudioLoadRequests"] = g_audio::flushAudioLoadRequests;

	//particle master
	kaguya["addNewParticleType"] = g_particles::addNewParticleType;
	kaguya["getParticleType"] = g_particles::getParticleType;
	kaguya["setParticleTypeBloomFactor"] = g_particles::updateBloomFactor;
	kaguya["setParticleTypeMaxLife"] = g_particles::updateMaxLife;
	kaguya["setParticleTypeResponse"] = g_particles::updateTypeResponse;
	kaguya["setParticleTypeBoxDimension"] = g_particles::updateBoxDimension;
	kaguya["setParticleTypeBoxCenter"] = g_particles::updateBoxCenter;
	kaguya["setParticleTypeRotateToFace"] = g_particles::updateRotateToFace;
	kaguya["setParticleTypeFeatherness"] = g_particles::updateFeatherness;
	kaguya["setParticleTypeWeight"] = g_particles::updateWeight;
	kaguya["clearAllParticles"] = g_particles::clearParticles;

	//graphics settings
	kaguya["setExposureTarget"] = g_graphicsSettings::setExposureTarget;
	kaguya["setExposureRate"] = g_graphicsSettings::setExposureRate;
	kaguya["setExposure"] = g_graphicsSettings::setExposure;
	kaguya["setBloomThreshold"] = g_graphicsSettings::setBloomThreshold;
	kaguya["setBloomDeviation"] = g_graphicsSettings::setBloomDeviation;

	//colour modulation settings
	kaguya["setColourModulationStrengths"] = g_colour_modulation::setStrengths;
	kaguya["setColourModulationForegroundColour"] = g_colour_modulation::l_setForegroundColour;
	kaguya["getColourModulationForegroundColour"] = g_colour_modulation::l_getForegroundColour;
	kaguya["setColourModulationBackgroundColour"] = g_colour_modulation::l_setBackgroundColour;
	kaguya["getColourModulationBackgroundColour"] = g_colour_modulation::l_getBackgroundColour;
	kaguya["setColourModulationForegroundCutoff"] = g_colour_modulation::setForegroundCutoff;
	kaguya["setColourModulationBackgroundCutoff"] = g_colour_modulation::setBackgroundCutoff;

}