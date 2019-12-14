#ifndef __COMPONENT_AUDIO__
#define __COMPONENT_AUDIO__

#include "component.h"
#include "constants.h"
#include "scriptable_class.h"
#include "audio_source.h"

class ComponentAudio : public Component, public ScriptableClass {
private:
	AudioSource source;
public:

	ComponentAudio() {

	}

	//plays an audio file
	void playAudio(const string& audioName) {
		source.playAudio(audioName);
	}

	//sets the repeat value
	void setRepeat(bool repeat) {
		source.setRepeat(repeat);
	}

	//al processing of audio component
	void alComponentHandle(map<string, AudioFile*>& audioFiles, const Point3& position) {
		source.alSideUpdate(audioFiles, position);
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentAudio"].setClass(kaguya::UserdataMetatable<ComponentAudio, Component>()
			.setConstructors<ComponentAudio()>()
			.addFunction("play", &ComponentAudio::playAudio)
			.addFunction("set_repeat", &ComponentAudio::setRepeat)
			.addOverloadedFunctions(
				"create",
				&ScriptableClass::create<ComponentAudio>
			)
			.addStaticFunction("type", &getType<ComponentAudio>)
			.addStaticFunction("cast", &Component::castDown<ComponentAudio>)
		);
	}
};

#endif