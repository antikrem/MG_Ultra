/** 
 * A Component that allowed
 */
#ifndef __COMPONENT_GRAPHICS__
#define __COMPONENT_GRAPHICS__

#include <atomic>
#include <shared_mutex>
#include "component.h"
#include "constants.h"

#include "graphics_state.h"

#include "scriptable_class.h"

class ComponentGraphics : public Component, public ScriptableClass<ComponentGraphics> {
private:
	shared_mutex lock;
	AnimationState state;

	// An internal visible sign
	atomic<bool> visible;
	// Sets the render space, true will be default 3d
	// False renders in 2d mode, using an orthogonal perspective
	atomic<bool> renderIn3D = true;

	// Wraping factor to use when texturing
	atomic<float> wrapFactor = 1.0f;

	// Rotaion value in degrees
	atomic<float> rotation = 0;

	// Transparency of this graphic
	// defaults at 1.0f;
	atomic<float> transparency = 1.0f;

public:
	ComponentGraphics() {
		visible.store(false);
	}

	ComponentGraphics(string animationSet) {
		visible.store(true);
		setAniamtionSet(animationSet, true);
	}

	ComponentGraphics(string animationSet, float scale) {
		visible.store(true);
		setAniamtionSet(animationSet, true);
		setScale(scale);
	}

	// Wrapper
	void l_setAniamtionSet(string name) {
		setAniamtionSet(name, true);
	}

	// Sets the animation type
	// If reset is true, sets the animation to idle, frame to zero and valid will be true
	void setAniamtionSet(string name, bool reset = true) {
		unique_lock<shared_mutex> lck(lock);
		state.animationSetName = name;
		if (reset) {
			state.valid = true;
			state.animationType = 1;
			state.currentFrame = 0;
		}
	}

	// Sets the animation state directly
	void setAnimationState(AnimationState state) {
		unique_lock<shared_mutex> lck(lock);
		this->state = state;
	}

	// 

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

	string getAnimationSet() {
		shared_lock<shared_mutex> lck(lock);
		return state.animationSetName;
	}

	// Set rotation value
	void setSpriteRotation(float angle) {
		this->rotation = angle;
	}

	// Get rotation value
	float getRotation() {
		return rotation;
	}

	// Set scale
	void setScale(float scale) {
		unique_lock<shared_mutex> lck(lock);
		this->state.scale = scale;
	}

	// Get scale
	float getScale() {
		unique_lock<shared_mutex> lck(lock);
		return this->state.scale;
	}

	// Set render in 3d
	void setRenderIn3D(bool renderIn3D) {
		this->renderIn3D = renderIn3D;
	}

	// Get render in 3d
	bool getRenderIn3D() {
		return renderIn3D;
	}

	// Set wrap factor
	void setWrapFactor(float wrapFactor) {
		this->wrapFactor = wrapFactor;
	}

	// Get wrap factor
	float getWrapFactor() {
		return wrapFactor;
	}

	// Set transparency (0 - 1)
	void setTransparency(float transparency) {
		this->transparency = transparency;
	}

	// Get transparency (0 - 1)
	float getTransparency() {
		return transparency;
	}

	// Set animation, not to be used 
	void setAnimationType(unsigned int animation) {
		unique_lock<shared_mutex> lck(lock);
		state.animationType = animation;
	}

	// Takes a position and returns a complete AnimationState
	AnimationState getAnimationState(bool* valid, Point3 pos) {
		auto temp = getAnimationState();
		*valid = temp.valid && temp.visible && visible.load();
		temp.transparency = transparency;
		temp.centerPostion = pos;
		temp.rotation = rotation;
		temp.wrapFactor = wrapFactor;
		return temp;
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentGraphics"].setClass(kaguya::UserdataMetatable<ComponentGraphics, Component>()
			.setConstructors<ComponentGraphics(string animationSet)>()
			.addFunction("set_animation_set", &ComponentGraphics::l_setAniamtionSet)
			.addFunction("get_animation_set", &ComponentGraphics::getAnimationSet)
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
			.addOverloadedFunctions(
				"create",
				&ScriptableClass::create<ComponentGraphics, string>,
				&ScriptableClass::create<ComponentGraphics, string, float>
			)
			.addStaticFunction("type", &getType<ComponentGraphics>)
			.addStaticFunction("cast", &Component::castDown<ComponentGraphics>)
		);
	}
};


#endif