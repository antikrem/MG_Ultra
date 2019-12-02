/*A system that pulls all directional lights and writes to a vao*/
#ifndef __SYSTEM_DIRECTIONAL_LIGHTS__
#define __SYSTEM_DIRECTIONAL_LIGHTS__

#include "system.h"

#include "component_directional_light.h"

#include "graphics_state.h"

class SystemDirectionalLight : public System {
	//Pointer to graphics state
	GraphicsState* graphicsState = nullptr;

	int boxCount = 0;
	DirectionalData* buffer = nullptr;

	int bufferSize = 0;

public:
	SystemDirectionalLight() {
		debugName = "s_directional_lights";

		target = SubPoolTarget(
			{ SubPoolComponents::ByComponents<ComponentDirectionalLight>() }
		);
	}

	void setGraphicsState(GraphicsState* graphicsState) {
		this->graphicsState = graphicsState;
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		//if buffer is null, dont write yet
		if (!buffer || (boxCount >= bufferSize)) {
			return;
		}

		buffer[boxCount++] = getComponent<ComponentDirectionalLight>(components)->getDirectionData();
	}

	void precycle(EntityPool* pool) override {
		boxCount = 0;
		buffer = graphicsState->getDirectionDataBuffer();
		bufferSize = graphicsState->getDirectionalDataBufferSize();
	}

	void postcycle(EntityPool* pool) override {
		graphicsState->commitDirectionalDataBuffer(boxCount);
	}

};

#endif