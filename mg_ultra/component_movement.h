/*A interface for finer details regarding movement
*/
#ifndef __COMPONENT_MOVEMENT__
#define __COMPONENT_MOVEMENT__

#include "component.h"
#include "scriptable_class.h"
#include "cus_struct2.h"
#include "math_ex.h"

class ComponentMovement : public Component, public ScriptableClass {
	//specifies this system to update in system_movement
	atomic<bool> updateInSystem = true;

	//cartesian  stuff
	atomic<Point3> velocity;
	atomic<Point3> acceleration;

	//polar movements
	atomic<float> angle;
	atomic<float> speed;

	atomic<float> angleChange;
	atomic<float> speedChange;

	atomic<float> speedCap = INFINITY;
	atomic<float> angleCap = INFINITY;

public:
	
	ComponentMovement() : velocity({ 0,0,0 }), acceleration({ 0,0,0 })  {
		angle = 0;
		speed = 0;

		angleChange = 0;
		speedChange = 0;
	}

	//updates all given values and returns new value
	Point3 getUpdatedPosition(Point3 position) {
		//move acceleration values to velocity
		velocity = acceleration.load() + velocity.load();

		speed.store(
			math_ex::clamp(speed + speedChange, -speedCap, speedCap.load())
		);
		angle.store(angle + angleChange);
		
		return position 
			+ velocity 
			+ Point3(Point2::generateFromPolar(speed, angle), 0);
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

	void setAngleChange(float angleChange) {
		this->angleChange = math_ex::clamp(angleChange, -angleCap, angleCap.load());
	}

	float getAngleChange() {
		return angleChange;
	}

	void setSpeedChange(float speedChange) {
		this->speedChange = speedChange;
	}

	float getSpeedChange() {
		return speedChange;
	}

	void setSpeedCap(float speedCap) {
		this->speedCap = speedCap;
	}

	float getSpeedCap() {
		return speedCap;
	}

	void setAngleCap(float angleCap) {
		this->angleCap = angleCap;
	}

	float getAngleCap() {
		return angleCap;
	}

	bool getUpdateInSystem() {
		return updateInSystem;
	}

	//stops all acceleration and speed changes 
	void pinVelocity() {
		acceleration = Point3(0,0,0);
		angleChange = 0;
		speedChange = 0;
	}

	//converts all velocity into polar part
	void pushVelocityToPolar() {
		Point3 temp = velocity.load() + Point3(Point2::generateFromPolar(speed, angle), 0);
		speed = temp.magnitude();
		angle = temp.getXY().getAng();
		velocity = Point3(0, 0, 0);
	}

	//gets angle of rotation
	float getDirection() {
		return (velocity.load() + Point3(Point2::generateFromPolar(speed, angle), 0)).getXY().getAng();
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
			.addFunction("set_angle_change", &ComponentMovement::setAngleChange)
			.addFunction("get_angle_change", &ComponentMovement::getAngleChange)
			.addFunction("set_speed_change", &ComponentMovement::setSpeedChange)
			.addFunction("get_speed_change", &ComponentMovement::getSpeedChange)

			.addFunction("set_speed_cap", &ComponentMovement::setSpeedCap)
			.addFunction("get_speed_cap", &ComponentMovement::getSpeedCap)
			.addFunction("set_angle_cap", &ComponentMovement::setAngleCap)
			.addFunction("get_angle_cap", &ComponentMovement::getAngleCap)

			.addFunction("set_update_manually", &ComponentMovement::updateManually)
			.addStaticFunction("create", ScriptableClass::create<ComponentMovement>)
			.addStaticFunction("type", &getType<ComponentMovement>)
			.addStaticFunction("cast", &Component::castDown<ComponentMovement>)
		);
	}

};

#endif
