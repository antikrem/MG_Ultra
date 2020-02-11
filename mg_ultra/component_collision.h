#ifndef __COMPONENT_COLLISION__
#define __COMPONENT_COLLISION__

#include "component.h"
#include "collision.h"
#include "scriptable_class.h"

//Allocates a collision to an entity
//Collidable box can take multiple forms
class ComponentCollision : public Component, public ScriptableClass {
private:
	//set to true when added to system
	bool addedToSystem = false;
	CollisionSpecification specification;

public:
	//default collision box of 10 units
	ComponentCollision()
		: specification(CollisionCircle(10)) {

	}

	ComponentCollision(float radius)
	: specification(CollisionCircle(radius)) {

	}

	ComponentCollision(float width, float height)
	: specification(CollisionRectangle(width, height)) {

	}

	//Updates collision position
	void update(Point3 position) {
		specification.updatePosition(position);
	}

	//returns reference to CollisionSpecification, while updating
	CollisionSpecification& getSpecification(const Point3& pos) {
		specification.updatePosition(pos);
		return specification;
	}

	//returns reference to CollisionSpecification, not updating
	CollisionSpecification& getSpecification() {
		return specification;
	}

	//returns if the entity this component is added to is in system
	//collision
	bool isAddedToSystem() {
		return addedToSystem;
	}

	//sets addedToSystem to true
	void markAddedToSystem() {
		addedToSystem = true;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentCollision"].setClass(kaguya::UserdataMetatable<ComponentCollision, Component>()
			.setConstructors<ComponentCollision()>()
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