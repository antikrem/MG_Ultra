/*Encapsulates a listener of audio
*/
#ifndef __COMPONENT_LISTENER__
#define __COMPONENT_LISTENER__

#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

#include "audio_listener.h"

class ComponentListener : public Component, public ScriptableClass {
	AudioListener audioListener;

public:
	void setTarget(const Point3& position) {
		return audioListener.setTarget(position);
	}

	Point3 getTarget() {
		return audioListener.getTarget();
	}


	void registerToLua(kaguya::State& state) override {
		state["ComponentListener"].setClass(kaguya::UserdataMetatable<ComponentListener, Component>()
			.setConstructors<ComponentListener()>()
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentListener>
			)
			.addStaticFunction("type", &getType<ComponentListener>)
			.addStaticFunction("cast", &Component::castDown<ComponentListener>)
		);
	}
};

#endif