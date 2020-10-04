#ifndef __COMPONENT_COLLISION__
#define __COMPONENT_COLLISION__

#include "component.h"
#include "collision.h"
#include "scriptable_class.h"

// Allocates a collision to an entity
// Collidable box can take multiple forms
class ComponentCollision : public Component, public ScriptableClass<ComponentCollision> {
private:
	// Set to true when added to system
	bool addedToSystem = false;
	CollisionSpecification specification;

public:
	// Default collision box of 10 units
	ComponentCollision()
		: specification(CollisionCircle(10)) {

	}

	ComponentCollision(float radius)
	: specification(CollisionCircle(radius)) {

	}

	ComponentCollision(float width, float height)
	: specification(CollisionRectangle(width, height)) {

	}

	// Updates collision position
	void update(Point3 position) {
		specification.updatePosition(position);
	}

	// Returns reference to CollisionSpecification, while updating
	CollisionSpecification& getSpecification(const Point3& pos) {
		specification.updatePosition(pos);
		return specification;
	}

	// Returns reference to CollisionSpecification, not updating
	CollisionSpecification& getSpecification() {
		return specification;
	}

	// Sets radius only if this is a circle collision
	void setCircleRadius(float radius) {
		if (specification.collisionType == CollisionTypes::CoTy_Circle) {
			specification.collidableInformation.circle.radius = radius;
		}
	}

	// Returns if the entity this component is added to is in system
	// collision
	bool isAddedToSystem() {
		return addedToSystem;
	}

	// Sets addedToSystem to true
	void markAddedToSystem() {
		addedToSystem = true;
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentCollision"].setClass(kaguya::UserdataMetatable<ComponentCollision, Component>()
			.setConstructors<ComponentCollision()>()
			.addFunction("set_circle_radius", &ComponentCollision::setCircleRadius)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentCollision, float>,
				ScriptableClass::create<ComponentCollision, float, float>
			)
			.addStaticFunction("type", &getType<ComponentCollision>)
			.addStaticFunction("cast", &Component::castDown<ComponentCollision>)
		);
	}
};

#endif