/*A interface for finer details regarding movement
*/
#ifndef __COMPONENT_MOVEMENT__
#define __COMPONENT_MOVEMENT__

#include "component.h"
#include "scriptable_class.h"
#include "cus_struct2.h"

class ComponentMovement : public Component, public ScriptableClass {
	//specifies this system to update in system_movement
	atomic<bool> updateInSystem = true;

	//cartesian  stuff
	atomic<Point3> velocity;
	atomic<Point3> acceleration;

	//polar movements
	atomic<float> angle;
	atomic<float> speed;

	atomic<float> angleVelocity;
	atomic<float> angleAcceleration;

	atomic<float> speedVelocity;
	atomic<float> speedAceleration;

	atomic<float> rotationVelocity = 0;

public:
	
	ComponentMovement() : velocity({ 0,0,0 }), acceleration({ 0,0,0 })  {
		angle = 0;
		speed = 0;

		angleVelocity = 0;
		angleAcceleration = 0;

		speedVelocity = 0;
		speedAceleration = 0;
	}

	//updates all given values and returns new value
	Point3 getUpdatedPosition(Point3 position) {
		//move acceleration values to velocity
		velocity = acceleration.load() + velocity.load();
		angleVelocity = angleVelocity + angleAcceleration;
		speedVelocity = speedVelocity + speedAceleration;

		speed.store(speed + speedVelocity);
		angle.store(angle + angleVelocity);

		return position + velocity + Point3(Point2::generateFromMagAng(speed, angle), 0);
	}

	//updates current rotation
	float getUpdatedRotation(float currentRotation) {
		return currentRotation + rotationVelocity;
	}

	void setRotationVelocity(float rotation) {
		rotationVelocity = rotation;
	}

	float getRotationVelocity() {
		return rotationVelocity;
	}

	tuple<float, float, float> lll_getUpdatedPosition(float x, float y, float z) {
		Point3 store = getUpdatedPosition( Point3(x, y, z) );
		return make_tuple(store.x, store.y, store.z);
	}

	tuple<float, float, float> ll_getUpdatedPosition(float x, float y) {
		Point3 store = getUpdatedPosition(Point3(x, y, 0));
		return make_tuple(store.x, store.y, store.z);
	}

	void setVelocity(Point3 velocity) {
		this->velocity = velocity;
	}

	void lll_setVelocity(float x, float y, float z) {
		this->velocity = Point3(x, y, z);
	}

	void ll_setVelocity(float x, float y) {
		this->velocity = Point3(x, y, velocity.load().z);
	}

	Point3 getVelocity() {
		return velocity;
	}

	tuple<float, float, float> lll_getVelocity() {
		Point3 temp = velocity.load();
		return make_tuple(temp.x, temp.y, temp.z);
	}

	void setAcceleration(Point3 acceleration) {
		this->acceleration = acceleration;
	}

	void lll_setAcceleration(float x, float y, float z) {
		this->acceleration = Point3(x, y, z);
	}

	void ll_setAcceleration(float x, float y) {
		this->acceleration = Point3(x, y, acceleration.load().z);
	}

	Point3 getAcceleration() {
		return acceleration;
	}

	tuple<float, float, float> lll_getAcceleration() {
		Point3 temp = acceleration.load();
		return make_tuple(temp.x, temp.y, temp.z);
	}

	void setAngle(float angle) {
		this->angle = angle;
	}

	float getAngle() {
		return angle;
	}

	void setSpeed(float speed) {
		this->speed = speed;
	}

	float getSpeed() {
		return speed;
	}

	void setAngleVelocity(float angleVelocity) {
		this->angleVelocity = angleVelocity;
	}

	float getAngleVelocity() {
		return angleVelocity;
	}

	void setAngleAcceleration(float angleAcceleration) {
		this->angleAcceleration = angleAcceleration;
	}

	float getAngleAcceleration() {
		return angleAcceleration;
	}

	void setSpeedVelocity(float speedVelocity) {
		this->speedVelocity = speedVelocity;
	}

	float getSpeedVelocity() {
		return speedVelocity;
	}

	void setSpeedAcceleration(float speedAceleration) {
		this->speedAceleration = speedAceleration;
	}

	float getSpeedAcceleration() {
		return speedAceleration;
	}

	bool getUpdateInSystem() {
		return updateInSystem;
	}

	void updateManually(bool value) {
		updateInSystem = !value;
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentMovement"].setClass(kaguya::UserdataMetatable<ComponentMovement, Component>()
			.setConstructors<ComponentMovement()>()
			.addOverloadedFunctions("get_updated_position", &ComponentMovement::lll_getUpdatedPosition, &ComponentMovement::ll_getUpdatedPosition)
			.addOverloadedFunctions("set_velocity", &ComponentMovement::ll_setVelocity, &ComponentMovement::lll_setVelocity)
			.addFunction("get_velocity", &ComponentMovement::lll_getVelocity)
			.addOverloadedFunctions("set_acceleration", &ComponentMovement::ll_setAcceleration, &ComponentMovement::lll_setAcceleration)
			.addFunction("get_acceleration", &ComponentMovement::lll_getAcceleration)
			.addFunction("set_angle", &ComponentMovement::setAngle)
			.addFunction("get_angle", &ComponentMovement::getAngle)
			.addFunction("set_speed", &ComponentMovement::setSpeed)
			.addFunction("get_speed", &ComponentMovement::getSpeed)
			.addFunction("set_angle_velocity", &ComponentMovement::setAngleVelocity)
			.addFunction("get_angle_velocity", &ComponentMovement::getAngleVelocity)
			.addFunction("set_angle_acceleration", &ComponentMovement::setAngleAcceleration)
			.addFunction("get_angle_acceleration", &ComponentMovement::getAngleAcceleration)
			.addFunction("set_speed_velocity", &ComponentMovement::setSpeedVelocity)
			.addFunction("get_speed_velocity", &ComponentMovement::getSpeedVelocity)
			.addFunction("set_speed_acceleration", &ComponentMovement::setSpeedAcceleration)
			.addFunction("get_speed_acceleration", &ComponentMovement::getSpeedAcceleration)
			.addFunction("get_rotation_speed", &ComponentMovement::getRotationVelocity)
			.addFunction("set_rotation_speed", &ComponentMovement::setRotationVelocity)
			.addFunction("set_update_manually", &ComponentMovement::updateManually)
			.addStaticFunction("create", ScriptableClass::create<ComponentMovement>)
			.addStaticFunction("type", &getType<ComponentMovement>)
			.addStaticFunction("cast", &Component::castDown<ComponentMovement>)
		);
	}

};

#endif
