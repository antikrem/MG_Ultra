#ifndef __COMPONENT_COLLISION__
#define __COMPONENT_COLLISION__

/*Defines a type of collision
There are two types of collisions,
A box collision and a cicle collision

*/
class CollisionBox {
private:
	//if not box, circle
	bool boxShape = false;
	//radius if boxShape is false, else one length of box
	float dim1=0;
	//if boxShape, the other length
	float dim2=0;
public:

};

//CollisionSystem will keep a list of ids of entities that touch this object
//Any system that uses collision to keep touch will use this interface
class ComponentCollision {

};

#endif