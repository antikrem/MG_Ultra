/*A Component that allowed */
#ifndef __COMPONENT_GRAPHICS__
#define __COMPONENT_GRAPHICS__

#include <atomic>
#include <shared_mutex>
#include "component.h"
#include "constants.h"

#include "graphics_state.h"

#include "scriptable_class.h"

class ComponentGraphics : public Component, public ScriptableClass {
private:
	shared_mutex lock;
	AnimationState state;

	//an internal visible sign
	atomic<bool> visible;
	//sets the render space, true will be default 3d
	//false renders in 2d mode, using an orthogonal perspective
	atomic<bool> renderIn3D = true;

	//rotaion value in degrees
	atomic<float> rotation = 0;

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
		setAniamtionSet(name, true);
	}

	//sets the animation type
	//if reset is true, sets the animation to idle, frame to zero and valid will be true
	void setAniamtionSet(string name, bool reset = true) {
		unique_lock<shared_mutex> lck(lock);
		state.animationSetName = name;
		if (reset) {
			state.valid = true;
			state.animationType = 1;
			state.currentFrame = 0;
		}
		this->state = state;
	}

	//sets the animation state directly
	void setAnimationState(AnimationState state) {
		unique_lock<shared_mutex> lck(lock);
		this->state = state;
	}

	void setVisible(bool visible) {
		this->visible = visible;
	}

	bool getVisible() {
		return visible;
	}

	AnimationState getAnimationState() {
		shared_lock<shared_mutex> lck(lock);
		return state;
	}

	//set rotation value
	void setRotation(float angle) {
		this->rotation = angle;
	}

	//get rotation value
	float getRotation() {
		return rotation;
	}

	//set scale
	void setScale(float scale) {
		unique_lock<shared_mutex> lck(lock);
		this->state.scale = scale;
	}

	//get scale
	float getScale() {
		unique_lock<shared_mutex> lck(lock);
		return this->state.scale;
	}

	//Takes a position and returns a complete AnimationState
	AnimationState getAnimationState(bool* valid, Point3 pos) {
		auto temp = getAnimationState();
		*valid = temp.valid && temp.visible && visible.load();
		temp.centerPostion = pos;
		temp.rotation = rotation;
		return temp;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentGraphics"].setClass(kaguya::UserdataMetatable<ComponentGraphics, Component>()
			.setConstructors<ComponentGraphics(string animationSet)>()
			.addFunction("set_animation_set", &ComponentGraphics::l_setAniamtionSet)
			.addFunction("set_visible", &ComponentGraphics::setVisible)
			.addFunction("set_rotation", &ComponentGraphics::setRotation)
			.addFunction("get_rotation", &ComponentGraphics::getRotation)
			.addFunction("set_scale", &ComponentGraphics::setScale)
			.addFunction("get_scale", &ComponentGraphics::getScale)
			.addStaticFunction("create", ScriptableClass::create<ComponentGraphics, string>)
			.addStaticFunction("type", &getType<ComponentGraphics>)
			.addStaticFunction("cast", &Component::castDown<ComponentGraphics>)
		);
	}
};


#endif