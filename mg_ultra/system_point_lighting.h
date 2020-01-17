/*A system that pulls all point lights and writes to a vao*/
#ifndef __SYSTEM_POINT_LIGHTS__
#define __SYSTEM_POINT_LIGHTS__

#include "system.h"

#include "component_point_light.h"

#include "graphics_state.h"

class SystemPointLight : public System {
	//Pointer to graphics state
	GraphicsState* graphicsState = nullptr;

	int lightCount = 0;
	PointLightData* buffer = nullptr;

	int bufferSize = 0;

public:
	SystemPointLight() {
		debugName = "s_point_lights";

		target = SubPoolTarget(
			{ SubPoolComponents::ByComponents<ComponentPointLight>() }
		);
	}

	void setGraphicsState(GraphicsState* graphicsState) {
		this->graphicsState = graphicsState;
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		//if buffer is null, dont write yet
		if (!buffer || (lightCount >= bufferSize)) {
			return;
		}

		buffer[lightCount++] = getComponent<ComponentPointLight>(components)->getPointData();
	}

	void precycle(EntityPool* pool) override {
		lightCount = 0;
		buffer = graphicsState->getPointLightDataBuffer();
		bufferSize = graphicsState->getPointLightDataBufferSize();
	}

	void postcycle(EntityPool* pool) override {
		graphicsState->commitPointLightDataBuffer(lightCount);
	}

};

#endif