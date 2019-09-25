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

		types.push_back(typeid(ComponentGraphics));
		types.push_back(typeid(ComponentAnimation));

		requiredTypes.push_back(typeid(ComponentGraphics));
		requiredTypes.push_back(typeid(ComponentAnimation));
	}

	void setAnimationMaster(AnimationsMaster* animationMaster) {
		this->animationMaster = animationMaster;
	}

	void handleComponentMap(map<type_index, Component*> components, int entityType, int id) override {
		ComponentGraphics* graphics = (ComponentGraphics*)components[typeid(ComponentGraphics)];
		ComponentAnimation* animation = (ComponentAnimation*)components[typeid(ComponentAnimation)];

		AnimationState state = graphics->getAnimationState();

		AnimationTemplate animationTemplate;
		bool valid = animationMaster->populateTemplate(state.animationSetName, state.animationType, &animationTemplate);

		if not(valid) {
			return;
		}

		//iterate frameskip, and see if it doubles around
		state.frameskipCounter = (state.frameskipCounter + 1) % animationTemplate.getFrameskip();
		
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