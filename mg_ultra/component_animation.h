/*Stores animation data, allows animations to be qued,
will be used to update graphics by system_animation*/
#ifndef __COMPONENT_ANIMATION__
#define __COMPONENT_ANIMATION__

#include <atomic>
#include <map>

#include "component.h"
#include "constants.h"

#include "graphics_state.h"

enum AnimationLevel {
	AL_default,
	AL_switch,
	AL_single
};

class ComponentAnimation : public Component {
	//There are three levels of animations
	//level 0: which is default animation looped, default to "idle" -> immutable
	//level 1: which is a switch animation, which will play looped until cleared or chanaged
	//level 2: one time single animation that will play to end
	unsigned int animationMap[3] = { 1, 0, 0 };

	//if true the switch animation is reset, used to change switch animation when a different switch is set
	atomic<bool> switchReset = false;

public:
	//Changes level 0/default animation
	void changeDefaultAnimation(unsigned int type) {
		animationMap[AL_default] = type;
	}

	//changes level 1/switch animation
	//if its a different animation, reset
	void changeSwitchAnimation(unsigned int type) {
		switchReset = (animationMap[AL_switch] != type);
		animationMap[AL_switch] = type;
	}

	//clears switch animation
	void clearSwitchAnimation() {
		animationMap[AL_switch] = 0;
	}

	//sets given single animation
	//resets animation is played animation is different
	void playSingleAnimation(unsigned int type) {
		animationMap[AL_single] = type;
	}

	void resetFlags() {
		switchReset = false;
	}

	bool getSwitchReset() {
		return switchReset;
	}

	//pulls animation, if zero then no animation has ben put
	//after being pulled, the animation will be set to zero
	unsigned int pullSingleAnimation() {
		unsigned int newAnimation = animationMap[AL_single];
		animationMap[AL_single] = 0;
		return newAnimation;
	}

	//returns current switch animation
	unsigned int getSwitchAnimation() {
		return animationMap[AL_switch];
	}

	//returns default
	unsigned int getDefaultAnimation() {
		return animationMap[AL_default];
	}
};

#endif