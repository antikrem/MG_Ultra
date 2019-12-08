#include "collision.h"
#include <map>

typedef bool(*CollisionEvaluater)(const CollisionSpecification&, const CollisionSpecification&);

//conducts collision evaluation on two circles
bool collisionEvaluationCircleCircle(const CollisionSpecification& first, const CollisionSpecification& second) {
	float distance = first.getPosition().getXY().distanceTo(second.getPosition().getXY());
	float threashold = first.collidableInformation.circle.radius + second.collidableInformation.circle.radius;

	return distance < threashold;
}

//map of appropiate Collision Evluaters to call
map<unsigned int, CollisionEvaluater> collisionEvaluations = {
	{ CoTy_Circle | CoTy_Circle, collisionEvaluationCircleCircle },
	{ CoTy_Circle | CoTy_Rectangle, collisionEvaluationCircleCircle },
	{ CoTy_Rectangle | CoTy_Rectangle, collisionEvaluationCircleCircle }
};

bool CollisionSpecification::isColliding(const CollisionSpecification& a, const CollisionSpecification& b) {
	if (!a.intialised || !b.intialised) {
		return false;
	}

	CollisionEvaluater fp = collisionEvaluations[a.collisionType | b.collisionType];
	//require correct ordering
	return a.collisionType < b.collisionType ?
		fp(a, b) :
		fp(b, a);

}