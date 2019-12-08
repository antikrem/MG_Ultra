#ifndef __COMPONENT_COLLISION__
#define __COMPONENT_COLLISION__

#include "component.h"
#include "collision.h"

//Allocates a collision to an entity
//Collidable box can take multiple forms
class ComponentCollision : public Component {
private:
	//set to true when added to system
	bool addedToSystem = false;
	CollisionSpecification specification;

public:
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

	//returns reference to CollisionSpecification
	CollisionSpecification& getSpecification() {
		return specification;
	}

	//returns if the entity this component is added to is in system
	//collision
	bool getAddedToSystem() {
		return addedToSystem;
	}

	//sets addedToSystem to true
	void setAddedToSystem() {
		addedToSystem = true;
	}

};

#endif