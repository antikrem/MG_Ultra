/*A system that pulls all graphics components and writes to a vao*/
#ifndef __SYSTEM_GRAPHICS__
#define __SYSTEM_GRAPHICS__

#include "system.h"

#include "component_position.h"
#include "component_graphics.h"
#include "component_text.h"
#include "component_camera.h"

class SystemGraphics : public System {
	//Pointer to camera
	Camera* camera = nullptr;
	//Pointer to graphics state
	GraphicsState* graphicsState = nullptr;

	int boxCount = 0;
	BoxData* buffer = nullptr;

	int bufferSize = 0;

public:
	SystemGraphics() {
		debugName = "s_graphics";
		types.push_back(typeid(ComponentPosition));
		types.push_back(typeid(ComponentGraphics));
		types.push_back(typeid(ComponentText));

		requiredTypes.push_back(typeid(ComponentPosition));
	}

	void setGraphicsState(GraphicsState* graphicsState) {
		this->graphicsState = graphicsState;
	}

	void setCamera(Camera* camera) {
		this->camera = camera;
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, int entityType, int id) override {
		//if buffer is null, dont write yet
		if (!buffer || (boxCount >= bufferSize)) {
			return;
		}

		//if both position and graphics components are present, this is a simple single state push case 
		if (components.count(typeid(ComponentGraphics)) ) {
			auto pos = getComponent<ComponentPosition>(components);
			auto gra = getComponent<ComponentGraphics>(components);

			bool toDraw;
			auto state = gra->getAnimationState(&toDraw, pos->getPosition3());

			if (toDraw) {
				buffer[boxCount] = graphicsState->evaluateToBox(state, 1.0f, gra->getRenderIn3D());
				boxCount++;
			}
		}
		//if otherwise a text component is found, get cached frames
		else if (components.count(typeid(ComponentText)) ) {
			auto pos = getComponent<ComponentPosition>(components);
			auto tex = getComponent<ComponentText>(components);
			//exit early if not visible
			if not(tex->getVisible()) {
				return;
			}

			//pull all the states 
			auto stateList = tex->getStates(pos->getPosition3());
		
			for (int i = 0; (i < (int)stateList.size()) && (boxCount < bufferSize); i++) {
				buffer[boxCount++] = graphicsState->evaluateToBox(stateList[i], tex->getScale(), tex->getRenderIn3D());
			}
		}
	}

	void precycle(EntityPool* pool) override {
		boxCount = 0;
		buffer = graphicsState->getBoxDataBuffer();
		bufferSize = graphicsState->getBoxDataBufferSize();
	}

	void postcycle(EntityPool* pool) override {
		//need to move camera entity values to camera
		auto cameraEnt = pool->getCachedEnt(ETCamera);
		if (cameraEnt) {
			auto cCamera = cameraEnt->getComponent<ComponentCamera>();
			auto cPosition = cameraEnt->getComponent<ComponentPosition>();

			camera->updateCamera(
				cPosition->getPosition3().getVec3(),
				cCamera->getViewTarget(),
				cCamera->getFOV()
			);
		}

		graphicsState->commitBoxDataBuffer(boxCount);
	}

};

#endif