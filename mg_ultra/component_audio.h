#ifndef __COMPONENT_AUDIO__
#define __COMPONENT_AUDIO__

#include <atomic>

#include "component.h"
#include "constants.h"
#include "scriptable_class.h"
#include "audio_source.h"

class ComponentAudio : public Component, public ScriptableClass<ComponentAudio> {
private:
	AudioSource source;

	// Set to true if there was a play request
	// that would be able to be stopped
	// consider it a neccesary (but not sufficient) condition
	// that audio is currently playing
	atomic<bool> playRequested = false;
public:

	ComponentAudio() {

	}

	ComponentAudio(string source) {
		playAudio(source);
	}

	// Sets to play a audio file
	void setSource(const string& audioName) {
		source.setQueuedAudio(audioName);
	}

	void playSource() {
		source.playQueuedAudio();
		playRequested = true;
	}

	// Plays an audio file
	void playAudio(const string& audioName) {
		source.playAudio(audioName);
		playRequested = true;
	}

	// Stop audio being played
	void stopAudio() {
		if (playRequested) {
			source.stopAudio();
			playRequested = false;
		}
	}

	// Pause this audio source
	void pauseAudio() {
		if (playRequested) {
			source.pauseAudio();
			playRequested = false;
		}
	}

	// Gets the current audio state
	int getAudioState() {
		return (int)source.getState();
	}

	// Sets the repeat value
	void setRepeat(bool repeat) {
		source.setRepeat(repeat);
	}

	// Sets the gain value
	void setGain(float gain) {
		source.setGain(gain);
	}

	// Al processing of audio component
	void alComponentHandle(map<string, AudioFile*>& audioFiles, const Point3& position) {
		source.alSideUpdate(audioFiles, position);
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentAudio"].setClass(kaguya::UserdataMetatable<ComponentAudio, Component>()
			.setConstructors<ComponentAudio()>()
			.addFunction("play", &ComponentAudio::playAudio)
			.addFunction("play_source", &ComponentAudio::playSource)
			.addFunction("resume", &ComponentAudio::playSource)
			.addFunction("stop", &ComponentAudio::stopAudio)
			.addFunction("stop_source", &ComponentAudio::stopAudio)
			.addFunction("pause", &ComponentAudio::pauseAudio)
			.addFunction("pause_source", &ComponentAudio::pauseAudio)
			.addFunction("set_source", &ComponentAudio::setSource)
			.addFunction("set_repeat", &ComponentAudio::setRepeat)
			.addFunction("set_gain", &ComponentAudio::setGain)
			.addFunction("get_state", &ComponentAudio::getAudioState)
			.addOverloadedFunctions(
				"create",
				&ScriptableClass::create<ComponentAudio>,
				&ScriptableClass::create<ComponentAudio, string>
			)
			.addStaticFunction("type", &getType<ComponentAudio>)
			.addStaticFunction("cast", &Component::castDown<ComponentAudio>)
		);
	}
};

#endif