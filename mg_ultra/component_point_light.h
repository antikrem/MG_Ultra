/*Allows an entity to cast a directional light*/
#ifndef __COMPONENT_POINT_LIGHT__
#define __COMPONENT_POINT_LIGHT__

#include "component.h"
#include "cus_struct2.h"

#include "scriptable_class.h"
#include "point_light.h"

#include "glm/glm.hpp"

class ComponentPointLight : public Component, public ScriptableClass<ComponentPointLight> {
	PointLight pointLight;

public:
	ComponentPointLight()
		: pointLight(Point3(0.0f), Point3(0.0f)) {
	}

	ComponentPointLight(const Point3& position, const Point3& colour)
		: pointLight(position, colour) {
	}

	ComponentPointLight(float r, float g, float b)
		: pointLight(Point3(0, 0, 0), Point3(r, g, b)) {
	}

	ComponentPointLight(float r, float g, float b, float aP, float bP, float cP)
		: pointLight(Point3(0, 0, 0), Point3(r, g, b)) {
		setParameters(aP, bP, cP);
	}

	PointLightData getPointData() {
		return pointLight.getData();
	}

	//set position of internal point light
	void setPointLightPosition(Point3 position) {
		pointLight.setPosition(position);
	}

	//sets colour
	void setColour(const Point3& colour) {
		pointLight.setColour(colour);
	}

	void l_setColour(float r, float g, float b) {
		pointLight.setColour(Point3(r, g, b));
	}

	void setParameters(float a, float b, float c) {
		pointLight.setConstants(a, b, c);
	}

	void setStrength(float strength) {
		pointLight.setStrength(strength);
	}

	void l_setStrength(float target, float rate) {
		pointLight.setStrength(target, rate);
	}

	void setByPoints(const Point2& p1, const Point2& p2, const Point2& p3) {
		glm::vec3 Y = glm::vec3(1.0f / p1.y, 1.0f / p2.y, 1.0f / p3.y);
		glm::mat4 A = glm::mat3(
			glm::vec3(SQUARE(p1.x), p1.x, 1.0f),
			glm::vec3(SQUARE(p2.x), p2.x, 1.0f),
			glm::vec3(SQUARE(p3.x), p3.x, 1.0f)
		);

		auto inversion = glm::inverse(A);

		setParameters(
			Y[0] * inversion[0][0] + Y[1] * inversion[0][1] + Y[2] * inversion[0][2],
			Y[0] * inversion[1][0] + Y[1] * inversion[1][1] + Y[2] * inversion[1][2],
			Y[0] * inversion[2][0] + Y[1] * inversion[2][1] + Y[2] * inversion[2][2]
		);
	}

	void l_setByPoints(float x0, float y0, float x1, float y1, float x2, float y2) {
		Point2 p0(x0, y0);
		Point2 p1(x1, y1);
		Point2 p2(x2, y2);

		setByPoints(p0, p1, p2);
	}

	void update() {
		pointLight.update();
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentPointLight"].setClass(kaguya::UserdataMetatable<ComponentPointLight, Component>()
			.setConstructors<ComponentPointLight()>()
			.addFunction("set_colour", &ComponentPointLight::l_setColour)
			.addFunction("set_parameters", &ComponentPointLight::setParameters)
			.addFunction("set_by_points", &ComponentPointLight::setByPoints)
			.addOverloadedFunctions(
				"set_strength", 
				&ComponentPointLight::setStrength,
				&ComponentPointLight::l_setStrength
			)
			.addOverloadedFunctions(
				"create",
				ScriptableClass::create<ComponentPointLight>,
				ScriptableClass::create<ComponentPointLight, float, float, float>,
				ScriptableClass::create<ComponentPointLight, float, float, float, float, float, float>
			)
			.addStaticFunction("type", &getType<ComponentPointLight>)
			.addStaticFunction("cast", &Component::castDown<ComponentPointLight>)
		);
	}
};

#endif