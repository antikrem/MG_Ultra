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

	//wraping factor to use when texturing
	atomic<float> wrapFactor = 1.0f;

	//rotaion value in degrees
	atomic<float> rotation = 0;

	//transparency of this graphic
	//defaults at 1.0f;
	atomic<float> transparency = 1.0f;

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

	//set render in 3d
	void setRenderIn3D(bool renderIn3D) {
		this->renderIn3D = renderIn3D;
	}

	//get render in 3d
	bool getRenderIn3D() {
		return renderIn3D;
	}

	//set wrap factor
	void setWrapFactor(float wrapFactor) {
		this->wrapFactor = wrapFactor;
	}

	//get wrap factor
	float getWrapFactor() {
		return wrapFactor;
	}

	//set transparency (0 - 1)
	void setTransparency(float transparency) {
		this->transparency = transparency;
	}

	//get transparency (0 - 1)
	float getTransparency() {
		return transparency;
	}

	//set animation, not to be used 
	void setAnimationType(unsigned int animation) {
		unique_lock<shared_mutex> lck(lock);
		state.animationType = animation;
	}

	//Takes a position and returns a complete AnimationState
	AnimationState getAnimationState(bool* valid, Point3 pos) {
		auto temp = getAnimationState();
		*valid = temp.valid && temp.visible && visible.load();
		temp.transparency = transparency;
		temp.centerPostion = pos;
		temp.rotation = rotation;
		temp.wrapFactor = wrapFactor;
		return temp;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentGraphics"].setClass(kaguya::UserdataMetatable<ComponentGraphics, Component>()
			.setConstructors<ComponentGraphics(string animationSet)>()
			.addFunction("set_animation_set", &ComponentGraphics::l_setAniamtionSet)
			.addFunction("set_animation", &ComponentGraphics::setAnimationType)
			.addFunction("set_visible", &ComponentGraphics::setVisible)
			.addFunction("set_scale", &ComponentGraphics::setScale)
			.addFunction("get_scale", &ComponentGraphics::getScale)
			.addFunction("set_render_in_3D", &ComponentGraphics::setRenderIn3D)
			.addFunction("get_render_in_3D", &ComponentGraphics::getRenderIn3D)
			.addFunction("set_wrap_factor", &ComponentGraphics::setWrapFactor)
			.addFunction("get_wrap_factor", &ComponentGraphics::getWrapFactor)
			.addFunction("set_transparency", &ComponentGraphics::setTransparency)
			.addFunction("get_transparency", &ComponentGraphics::getTransparency)
			.addStaticFunction("create", ScriptableClass::create<ComponentGraphics, string>)
			.addStaticFunction("type", &getType<ComponentGraphics>)
			.addStaticFunction("cast", &Component::castDown<ComponentGraphics>)
		);
	}
};


#endif