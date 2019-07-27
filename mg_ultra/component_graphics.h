/*A Component that allowed */
#ifndef __COMPONENT_GRAPHICS__
#define __COMPONENT_GRAPHICS__

#include <atomic>
#include "component.h"
#include "constants.h"

#include "graphics_state.h"

#include "scriptable_class.h"

class ComponentGraphics : public Component, public ScriptableClass {
private:
	//todo, lock should be enough/better
	TrippleBuffer<AnimationState> states;

	//an internal visible sign
	atomic<bool> visible;

public:
	ComponentGraphics() {
		visible.store(false);
	}

	ComponentGraphics(string animationSet) {
		visible.store(true);
		setAniamtionSet(animationSet, true);
	}

	//wrapper
	void l_setAniamtionSet(string name) {
		setAniamtionSet(name);
	}

	//sets the animation type
	//if reset is true, sets the animation to true, frame to zero and valid will be true
	void setAniamtionSet(string name, bool reset = true) {
		AnimationState state;
		state.animationSetName = name;
		if (reset) {
			state.valid = true;
			state.animationType = 1;
			state.currentFrame = 0;
		}
		states.store(state);
	}

	//sets the animation state directly
	void setAnimationState(AnimationState state) {
		states.store(state);
	}

	void setVisible(bool visible) {
		this->visible = visible;
	}

	AnimationState getAnimationState() {
		return states.load();
	}

	//Takes a position and returns a complete AnimationState
	AnimationState getAnimationState(bool* valid, Point3 pos) {
		auto temp = getAnimationState();
		*valid = temp.valid && temp.visible && visible.load();
		temp.centerPostion = pos;
		return temp;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentGraphics"].setClass(kaguya::UserdataMetatable<ComponentGraphics, Component>()
			.setConstructors<ComponentGraphics(string animationSet)>()
			.addFunction("set_animation_set", &ComponentGraphics::l_setAniamtionSet)
			.addStaticFunction("type", &getType<ComponentGraphics>)
			.addStaticFunction("cast", &Component::castDown<ComponentGraphics>)
		);
	}
};


#endif