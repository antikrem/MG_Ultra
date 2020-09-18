/*Camera is handled by a script 300 times a second
the script loaded is in script/camera.lua
*/
#ifndef __COMPONENT_CAMERA__
#define __COMPONENT_CAMERA__

#include "component.h"
#include "cus_struct2.h"
#include "script_master.h"
#include "scriptable_class.h"

class ComponentCamera : public Component, public ScriptableClass<ComponentCamera> {
	// Wrapping camera
	Camera* camera = nullptr;

	//treats location as a vector
	atomic<Point3> viewTarget;

public:

	ComponentCamera() 
			: viewTarget{ Point3(0,0,0) } {
	}

	ComponentCamera(Camera* camera) 
			: viewTarget{ Point3(0,0,0) } {
		this->camera = camera;
	}
	
	glm::vec3 getViewTarget() {
		return viewTarget.load().getVec3();
	}

	tuple<float, float, float> l_getViewTarget() {
		return tuple<float, float, float>(viewTarget.load().x, viewTarget.load().y, viewTarget.load().z);
	}

	void setViewTarget(Point3 target) {
		viewTarget = target;
	}

	void lll_setViewTarget(float x, float y, float z) {
		viewTarget = Point3(x, y, z);
	}

	void ll_setViewTarget(float x, float y) {
		viewTarget = Point3(x, y, this->viewTarget.load().z);
	}

	void l_addViewTarget(float x, float y, float z) {
		viewTarget = viewTarget.load() + Point3(x, y, z);
	}

	void setFOV(float fov) {
		camera->setFOV(fov);
	}

	float getFOV() {
		return camera->getFOV();
	}

	float getClipNear() {
		return camera->getClipNear();
	}

	void setClipNear(float clipNear) {
		camera->setClipNear(clipNear);
	}

	float getClipFar() {
		return camera->getClipFar();
	}

	void setClipFar(float clipFar) {
		camera->setClipFar(clipFar);
	}


	static void registerToLua(kaguya::State& state) {
		state["ComponentCamera"].setClass(kaguya::UserdataMetatable<ComponentCamera, Component>()
			.setConstructors<ComponentCamera()>()
			.addOverloadedFunctions("set_view_target", &ComponentCamera::ll_setViewTarget, &ComponentCamera::lll_setViewTarget)
			.addFunction("get_view_target", &ComponentCamera::l_getViewTarget)
			.addFunction("get_fov", &ComponentCamera::getFOV)
			.addFunction("set_fov", &ComponentCamera::setFOV)
			.addFunction("get_clip_near", &ComponentCamera::getClipNear)
			.addFunction("set_clip_cear", &ComponentCamera::setClipNear)
			.addFunction("get_clip_far", &ComponentCamera::getClipFar)
			.addFunction("set_clip_far", &ComponentCamera::setClipFar)
			.addStaticFunction("type", &getType<ComponentCamera>)
			.addStaticFunction("cast", &Component::castDown<ComponentCamera>)
		);
	}

};


#endif