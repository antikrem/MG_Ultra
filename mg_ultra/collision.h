#ifndef __COLLISION__
#define __COLLISION__

#include "cus_struct2.h"
#include <atomic>

//enumeration of collidable types
enum CollisionTypes {
	CoTy_Circle = 0b01,
	CoTy_Rectangle = 0b10
};


/*Definition for a collidable circle
*/
struct CollisionCircle {
	float radius;

	CollisionCircle(float radius) {
		this->radius = radius;
	}
};

/*Definition for a collidable box
*/
struct CollisionRectangle {
	float width;
	float height;

	CollisionRectangle(float width, float height) {
		this->width = width;
		this->height = height;
	}
};

/*Definition of object used for containing collidable object*/
typedef union CollisionBound {
	CollisionCircle circle;
	CollisionRectangle rectangle;

	CollisionBound(CollisionCircle circle) {
		this->circle = circle;
	}

	CollisionBound(CollisionRectangle rectangle) {
		this->rectangle = rectangle;
	}

} CollisionBound;

//contains all the data required for collision
class CollisionSpecification {
private:
	atomic<float> x, y, z;

public:
	//set to true when position is set
	atomic<bool> intialised = false;
	
	CollisionTypes collisionType;
	CollisionBound collidableInformation;

	CollisionSpecification(CollisionCircle circle)
		: collidableInformation(circle) {
		this->collisionType = CoTy_Circle;
	}

	CollisionSpecification(CollisionRectangle rectangle)
		: collidableInformation(rectangle) {
		this->collisionType = CoTy_Rectangle;
	}

	void updatePosition(const Point3& position) {
		intialised = true;
		this->x = position.x;
		this->y = position.y;
		this->z = position.z;
	}

	//returns a point 3 of position
	Point3 getPosition() const {
		return Point3(x, y, z);
	}

	//compares two CollisionSpecification
	//return true if a collision occured
	static bool isColliding(const CollisionSpecification& a, const CollisionSpecification& b);
};



#endif