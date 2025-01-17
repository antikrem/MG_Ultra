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

	//specifies if this particle looks towards target
	bool rotateToFace = false;

	float velocityRotation = 0.0f;

	float feathernessMean = 0.1f;
	float feathernessDeviation = 0.025f;

	float scrollness = 0.0f;

	float weightMean = 1.0f;
	float weightDeviation = 0.1f;

	int frameSkip;
	int frameCount;

	//Will randomly add a factor between [1, lengthdeviation] to particle life
	float maxLifeDeviation = 1.0f;

	float bloomFactor = 0.0f; 

	float scale = 1.0f;

	//animation information is cached within the particle type
	//each box is a template for creating a box for a particle
	vector<BoxData> boxes;

	//specifies the bounding box width, height and depth
	//specifies half the full length
	Point3 boundingBoxDimension = Point3(0);
	//middle point of the bounding box
	Point3 boundingBoxPosition = Point3(0);
	//Response for bounding box
	ParticleBoxResponse boxResponse = ParticleBoxResponse::Nothing;

	ParticleType() {

	}

	ParticleType(string animationSet, AnimationsMaster* animationMaster, int animation, float scale) {
		this->animation = animationSet;
		this->scale = scale;
		AnimationState baseState(animationSet, animation, 0, 0);

		auto animationPtr = animationMaster->getAnimationTemplate(animationSet, animation);

		if (!animationPtr) {
			return;
		}

		baseState.minimumAmbient = bloomFactor;

		frameCount = animationPtr->getNumberOfFrames();

		for (int i = 0; i < frameCount; i++) {
			baseState.currentFrame = i;
			boxes.push_back(animationMaster->evaluateToBox(baseState, scale));
		}

		frameSkip = animationPtr->getFrameskip() ? animationPtr->getFrameskip() : INT_MAX;

		wellformed = true;
	}

	//takes a reference to a particle and returns the associated box
	//returns true if valid and ready to spawn
	bool evaluateBox(ParticleSpecification& particle, BoxData& box) {
		if (!particle.lifetimeFactor || !frameSkip) {
			return false;
		}

		int frame = (int)(particle.lifetime / particle.lifetimeFactor) / frameSkip;
		if (frame >= frameCount) {
			return false;
		}

		box = boxes[frame];
		box.xyz[0] = particle.x;
		box.xyz[1] = particle.y;
		box.xyz[2] = particle.z;
		box.rotation = particle.rotation;
		return true;
	}

	//sets the bloom factor for this particle
	void setBloomFactor(float bloomFactor) {
		this->bloomFactor = bloomFactor;

		for (auto& i : boxes) {
			i.ambientMin = bloomFactor;
		}
	}

	//sets response box half dimension
	void setResponseBoxDimension(const Point3& box) {
		boundingBoxDimension = box;
	}

	//sets response box center position
	void setResponseBoxCenter(const Point3& box) {
		boundingBoxPosition = box;
	}

	//sets is this type rotates to face
	void setRotateToBox(bool rotateToFace) {
		this->rotateToFace = rotateToFace;
	}

	//returns true if position is within bounding box
	bool checkBoundingBox(const Point3& pos) {
		return
			abs(pos.x - boundingBoxPosition.x) < boundingBoxDimension.x
			&& abs(pos.y - boundingBoxPosition.y) < boundingBoxDimension.y
			&& abs(pos.z - boundingBoxPosition.z) < boundingBoxDimension.z;
	}

	//wraps position
	void wrapPosition(Particle& particle) {
		if (abs(particle.position.x - boundingBoxPosition.x) > boundingBoxDimension.x) {
			if (particle.position.x - boundingBoxPosition.x > boundingBoxDimension.x) {
				particle.position.x = particle.position.x - 2 * boundingBoxDimension.x;
			}
			else {
				particle.position.x = particle.position.x + 2 * boundingBoxDimension.x;
			}
		}
		if (abs(particle.position.y - boundingBoxPosition.y) > boundingBoxDimension.y) {
			if (particle.position.y - boundingBoxPosition.y > boundingBoxDimension.y) {
				particle.position.y = particle.position.x - 2 * boundingBoxDimension.y;
			}
			else {
				particle.position.y = particle.position.y + 2 * boundingBoxDimension.y;
			}
		}
		if (abs(particle.position.z - boundingBoxPosition.z) > boundingBoxDimension.z) {
			if (particle.position.z - boundingBoxPosition.z > boundingBoxDimension.z) {
				particle.position.z = particle.position.z - 2 * boundingBoxDimension.z;
			}
			else {
				particle.position.z = particle.position.z + 2 * boundingBoxDimension.z;
			}
		}
	}

	ParticleTypeSpecification getParticleTypeSpecification() {
		return ParticleTypeSpecification(
			feathernessMean,
			feathernessDeviation,
			weightMean,
			weightDeviation,
			maxLifeDeviation,
			rotateToFace,
			velocityRotation,
			boundingBoxDimension,
			boundingBoxPosition,
			boxResponse
		);
	}
};

#endif