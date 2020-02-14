/*A system that pulls all graphics components and writes to a vao*/
#ifndef __SYSTEM_GRAPHICS__
#define __SYSTEM_GRAPHICS__

#include "system.h"

#include "component_position.h"
#include "component_graphics.h"
#include "component_text.h"
#include "component_camera.h"
#include "component_transparency.h"
#include "component_min_ambient.h"
#include "component_collision.h"

#include "camera.h"
#include "graphics_state.h"

#include "particle_master.h"

class SystemGraphics : public System {
	//Pointer to camera
	Camera* camera = nullptr;
	//Pointer to graphics state
	GraphicsState* graphicsState = nullptr;

	int boxCount = 0;
	BoxData* buffer = nullptr;
	int bufferSize = 0;

	int uiBoxCount = 0;
	BoxData* uiBuffer = nullptr;
	int uiBufferSize = 0;

	//pointer to particle master
	atomic<ParticleMaster*> particleMaster = nullptr;

	//debug render hitbox mode
	bool debugRenderHitbox = false;

public:
	SystemGraphics() {
		debugName = "s_graphics";

		target = SubPoolTarget(
			{ SubPoolComponents::ByComponents<ComponentPosition, ComponentGraphics>(),
			SubPoolComponents::ByComponents<ComponentPosition, ComponentText>() }
		);
	}

	void setGraphicsState(GraphicsState* graphicsState) {
		this->graphicsState = graphicsState;
	}

	void setCamera(Camera* camera) {
		this->camera = camera;
	}

	void setParticleMaster(ParticleMaster* particleMaster) {
		this->particleMaster = particleMaster;
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		//if both position and graphics components are present, this is a simple single state push case 
		if (components.count(typeid(ComponentGraphics)) ) {
			auto pos = getComponent<ComponentPosition>(components);
			auto gra = getComponent<ComponentGraphics>(components);
			auto tra = getComponent<ComponentTransparency>(components);
			auto amb = getComponent<ComponentMinAmbient>(components);

			bool toDraw;
			AnimationState state = gra->getAnimationState(&toDraw, pos->getPosition3());
			
			if (tra) {
				state.transparency = tra->getCurrent();
			}

			if (amb) {
				state.minimumAmbient = amb->getValue();
			}

			if (toDraw && boxCount < bufferSize) {
				if (gra->getRenderIn3D()) {
					buffer[boxCount] = graphicsState->evaluateToBox(state, 1.0f);
					boxCount++;
				}
				else {
					uiBuffer[uiBoxCount] = graphicsState->evaluateToBox(state, 1.0f);
					uiBoxCount++;
				}
			}
		}
		//if otherwise a text component is found, get cached frames
		else if (components.count(typeid(ComponentText)) ) {
			auto pos = getComponent<ComponentPosition>(components);
			auto tex = getComponent<ComponentText>(components);
			auto tra = getComponent<ComponentTransparency>(components);

			//exit early if not visible
			if not(tex->getVisible()) {
				return;
			}

			//pull all the states 
			auto stateList = tex->getStates(pos->getPosition3());
		
			if (tra) {
				float transparency = tra->getCurrent();
				for (int i = 0; i < (int)stateList.size(); i++) {
					stateList[i].transparency = transparency;
				}
			}

			if (tex->getRenderIn3D()) {
				for (int i = 0; (i < (int)stateList.size()) && (boxCount < bufferSize); i++) {
					buffer[boxCount++] = graphicsState->evaluateToBox(stateList[i], tex->getScale());
				}
			}
			else {
				for (int i = 0; (i < (int)stateList.size()) && (uiBoxCount < uiBufferSize); i++) {
					uiBuffer[uiBoxCount++] = graphicsState->evaluateToBox(stateList[i], tex->getScale());
				}
			}
		}


		if (debugRenderHitbox) {
			auto col = getComponent<ComponentCollision>(components);
			auto pos = getComponent<ComponentPosition>(components);
			if (col) {
				auto& spec = col->getSpecification(pos->getPosition3());

				AnimationState hitbox;
				hitbox.centerPostion = spec.getPosition();
				hitbox.centerPostion.z = hitbox.centerPostion.z - 0.5f;
				hitbox.animationSetName = "debug_hitbox";
				hitbox.animationType = 1;
				hitbox.currentFrame = 0;
				hitbox.frameskipCounter = 0;
				hitbox.minimumAmbient = 1.0f;
				hitbox.transparency = 0.5f;

				hitbox.scale = spec.collidableInformation.circle.radius / 50.0f;

				buffer[boxCount] = graphicsState->evaluateToBox(hitbox, 1.0f);
				boxCount++;
			}
		}
	}

	void precycle(EntityPool* pool) override {
		boxCount = 0;
		buffer = graphicsState->getBoxDataBuffer();
		bufferSize = graphicsState->getBoxDataBufferSize();

		uiBoxCount = 0;
		uiBuffer = graphicsState->getUIBoxDataBuffer();
		uiBufferSize = graphicsState->getUIBoxDataBufferSize();

		debugRenderHitbox = registar->get("debug_render_hitbox", &debugRenderHitbox) && debugRenderHitbox;
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

		//transfer particles
		if (particleMaster.load()) {
			boxCount = particleMaster.load()->copyOutBoxes(buffer, boxCount, bufferSize);
		}

		graphicsState->commitBoxDataBuffer(boxCount);
		graphicsState->commitUIBoxDataBuffer(uiBoxCount);
	}

};

#endif