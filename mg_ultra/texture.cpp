#include "texture.h"

//A global AnimationsMaster used to texture querying
AnimationsMaster* g_animationMaster = nullptr;

AnimationsMaster::AnimationsMaster() {
	g_animationMaster = this;
	memset(loadedTextures, 0, MAX_TEXTURE_COUNT * sizeof(int));
}

#define INVALID_POINT make_tuple(-1, -1);

tuple<int, int> g_aniquery::getAnimationSize(string animationSet, int animationType) {
	if (!g_animationMaster) {
		return INVALID_POINT;
	}
	
	auto set = g_animationMaster->getAnimationSetTemplate(animationSet);
	if (!set) {
		return INVALID_POINT;
	}

	auto ani = set->getAnimationTemplate((unsigned int)animationType);
	if (!ani) {
		return INVALID_POINT;
	}

	return make_tuple(ani->getWidth(), ani->getHeight());
}

