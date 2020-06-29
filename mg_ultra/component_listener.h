/*Encapsulates a listener of audio
*/
#ifndef __COMPONENT_LISTENER__
#define __COMPONENT_LISTENER__

#include <tuple>

#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

#include "audio_listener.h"

class ComponentListener : public Component, public ScriptableClass<ComponentListener> {
	AudioListener audioListener;

public:
	void setListenTarget(const Point3& position) {
		return audioListener.setListenTarget(position);
	}

	Point3 getListenTarget() {
		return audioListener.getListenTarget();
	}

	void l_setListenTarget(float x, float y, float z) {
		return setListenTarget(Point3(x, y, z));
	}

	tuple<float, float, float> l_getListenTarget() {
		Point3 target = getListenTarget();
		return make_tuple(target.x, target.y, target.z);
	}


	void setUpTarget(const Point3& position) {
		return audioListener.setUpTarget(position);
	}

	Point3 getUpTarget() {
		return audioListener.getUpTarget();
	}

	void l_setUpTarget(float x, float y, float z) {
		return setUpTarget(Point3(x, y, z));
	}

	tuple<float, float, float> l_getUpTarget() {
		Point3 target = getUpTarget();
		return make_tuple(target.x, target.y, target.z);
	}

	//updates associated listener
	void updateListener() {
		audioListener.updateListener();
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentListener"].setClass(kaguya::UserdataMetatable<ComponentListener, Component>()
			.setConstructors<ComponentListener()>()
			.addFunction("set_listen_target", &ComponentListener::l_setListenTarget)
			.addFunction("get_listen_target", &ComponentListener::l_getListenTarget)
			.addFunction("set_up_target", &ComponentListener::l_setUpTarget)
			.addFunction("get_up_target", &ComponentListener::l_getUpTarget)
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