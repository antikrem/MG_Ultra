/*A Component that is simply a position, that can be access in a safe way*/
#ifndef __COMPONENT_POSITION__
#define __COMPONENT_POSITION__

#include "component.h"
#include "cus_struct2.h"

#include "scriptable_class.h"

#include <atomic>


/*position is stored as Point2 + z value*/
class ComponentPosition : public Component, public ScriptableClass {
private:
	atomic<Point2> position = Point2(0,0);
	atomic<float> z;


public:
	ComponentPosition(Point3 xyz) {
		setPosition(xyz);
	}

	ComponentPosition(Point2 xy, float z = 0) {
		setPosition(xy, z);
	}

	ComponentPosition(float x, float y) {
		setPosition(x, y, 0);
	}

	ComponentPosition(float x, float y, float z) {
		setPosition(x, y, z);
	}

	ComponentPosition() {
		setPosition(0, 0, 0);
	}

	ComponentPosition(ComponentPosition* a) {
		setPosition(a->getPosition3());
	}

	void setPosition(Point3 xyz) {
		position = Point2(xyz.x, xyz.y);
		z = xyz.z;
	}

	void setPosition(Point2 xy, float z) {
		position = Point2(xy.x, xy.y);
		this->z = z;
	}

	void setPosition(float x, float y, float z) {
		position = Point2(x, y);
		this->z = z;
	}

	//wrapper for setposition(float, float ,float), allowing lua binding
	void lll_setPosition(float x, float y, float z) {
		setPosition(x, y, z);
	}

	//wrapper for setposition(float, float ,float), allowing lua binding
	void ll_setPosition(float x, float y) {
		setPosition(x, y, this->z);
	}

	Point2 getPosition2() {
		Point2 position = this->position;
		return position;
	}

	Point3 getPosition3() {
		Point3 position = Point3(this->position, z);
		return position;
	}

	//wrapper for get position allowing lua binding
	tuple<float, float, float> getPosition() {
		return tuple<float, float, float>(position.load().x, position.load().y, z);
	}

	//Adds to position, is more thread consistent then combining getPosition and setPosition
	void addPosition(Point3 pos) {
		position = Point2(pos.x, pos.y) + position.load();
		z = z + pos.z;
	}

	void ll_addPosition(float x, float y) {
		addPosition(Point3(x, y, 0));
	}

	void lll_addPosition(float x, float y, float z) {
		addPosition(Point3(x, y, z));
	}


	void addPosition(Point2 pos, float z = 0) {
		position = position.load() + pos;
		this->z = this->z + z;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentPosition"].setClass(kaguya::UserdataMetatable<ComponentPosition, Component>()
			.setConstructors<ComponentPosition(), ComponentPosition(float, float), ComponentPosition(float, float, float)>()
			.addOverloadedFunctions("set_position", &ComponentPosition::lll_setPosition, &ComponentPosition::ll_setPosition)
			.addFunction("get_position", &ComponentPosition::getPosition)
			.addOverloadedFunctions("add_position", &ComponentPosition::ll_addPosition, &ComponentPosition::lll_addPosition)
			.addOverloadedFunctions(
				"create", 
				ScriptableClass::create<ComponentPosition>,
				ScriptableClass::create<ComponentPosition, float, float>,
				ScriptableClass::create<ComponentPosition, float, float, float>
			)
			.addStaticFunction("type", &getType<ComponentPosition>)
			.addStaticFunction("cast", &Component::castDown<ComponentPosition>)
		);
	}
};

#endif