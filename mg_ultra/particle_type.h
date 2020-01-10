//Organises particle type
#ifndef __PARTICLE_TYPE__
#define __PARTICLE_TYPE__

#include <atomic>
#include <vector>

#include "texture.h"
#include "particle.h"

struct ParticleType {
	//set to true on good construction
	bool wellformed = false;

	string animation;
	float weightMean = 1.0f;
	float weightDeviation = 0.1f;

	int frameSkip;
	int frameCount;

	//Will randomly add a factor between [1, lengthdeviation] to particle life
	float maxLifeDeviation = 1.0f;

	float bloomFactor = 1.0f; 

	//animation information is cached within the particle type
	//each box is a template for creating a box for a particle
	vector<BoxData> boxes;

	ParticleType() {

	}

	ParticleType(const ParticleType& p) {
		this->animation = p.animation;
		this->weightMean = p.weightMean;
		this->weightDeviation = p.weightDeviation;

		this->boxes = p.boxes;
	}

	ParticleType(string animationSet, AnimationsMaster* animationMaster, int animation) {
		this->animation = animationSet;
		AnimationState baseState(animationSet, animation, 0, 0);

		auto animationPtr = animationMaster->getAnimationTemplate(animationSet, animation);

		if (!animationPtr) {
			return;
		}

		frameCount = animationPtr->getNumberOfFrames();

		for (int i = 0; i < frameCount; i++) {
			baseState.currentFrame = i;
			boxes.push_back(animationMaster->evaluateToBox(baseState, 1.0f));
		}

		frameSkip = animationPtr->getFrameskip() ? animationPtr->getFrameskip() : INT_MAX;

		wellformed = true;
	}

	//takes a reference to a particle and returns the associated box
	//returns true if valid and ready to spawn
	bool evaluateBox(ParticleSpecification& particle, BoxData& box) {
		int frame = (int)(particle.lifetime / particle.lifetimeFactor) / frameSkip;
		if (frame >= frameCount) {
			return false;
		}

		box = boxes[frame];
		box.xyz[0] = particle.x;
		box.xyz[1] = particle.y;
		box.xyz[2] = particle.z;
		return true;
	}

	//sets the bloom factor for this particle
	void setBloomFactor(float bloomFactor) {
		this->bloomFactor = bloomFactor;

		for (auto& i : boxes) {
			i.lightSensitivity[0] = bloomFactor;
			i.lightSensitivity[1] = bloomFactor;
			i.lightSensitivity[2] = bloomFactor;
		}
	}
};

#endif