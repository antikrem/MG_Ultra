/*Updates an entities component graphics if it has a component animation*/
#ifndef __SYSTEM_ANIMATION__
#define __SYSTEM_ANIMATION__

#include "system.h"
#include "component_graphics.h"
#include "component_animation.h"

class SystemAnimation : public System {
	//pointer to animation state
	AnimationsMaster* animationMaster = nullptr;

public:
	SystemAnimation() {
		debugName = "s_animation";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentGraphics, ComponentAnimation>()
		);
	}

	void setAnimationMaster(AnimationsMaster* animationMaster) {
		this->animationMaster = animationMaster;
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto graphics = getComponent<ComponentGraphics>(components);
		auto animation = getComponent<ComponentAnimation>(components);

		AnimationState state = graphics->getAnimationState();

		AnimationTemplate animationTemplate;
		bool valid = animationMaster->populateTemplate(state.animationSetName, state.animationType, &animationTemplate);

		if not(valid) {
			return;
		}

		//if animation has no frameskip, and is used in animation
		//treat as if it was 1 frameskip
		//iterate frameskip, and see if it doubles around
		int frameSkip = animationTemplate.getFrameskip();
		state.frameskipCounter = (state.frameskipCounter + 1) % (frameSkip ? frameSkip : 1);
		
		//handle switching animations
		//first, see if theres a request to play a single animation
		unsigned int newAnimation;
		if (newAnimation = animation->pullSingleAnimation()) {
			state.resetAnimation(newAnimation);
			state.inSingleAnimation = true;
		}
		//otherwise a switch animation might have changed
		//check the animation is not playing a single, and its reset flag is true
		else if (!state.inSingleAnimation && animation->getSwitchReset()) {
			state.resetAnimation(animation->getSwitchAnimation());
		}
		//if the frame counter has looped
		else if (state.frameskipCounter == 0) {
			//check if animation frame loops around
			state.currentFrame = (state.currentFrame + 1) % animationTemplate.getNumberOfFrames();
			//is the current frame has looped
			if (state.currentFrame == 0) {
				//animation will need to be swapped
				//remove any inAnimation flag to begin
				state.inSingleAnimation = false;

				//need to check if theres a switch animation
				if (newAnimation = animation->getSwitchAnimation()) {
					state.animationType = newAnimation;
				}
				//if there isnt a switch, fallback on default
				else {
					state.animationType = animation->getDefaultAnimation();
				}

			}
		}

		//set reset flags to zero
		animation->resetFlags();

		graphics->setAnimationState(state);
	}



};


#endif