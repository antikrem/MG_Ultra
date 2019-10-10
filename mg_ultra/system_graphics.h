/*A system that pulls all graphics components and writes to a vao*/
#ifndef __SYSTEM_GRAPHICS__
#define __SYSTEM_GRAPHICS__

#include "system.h"

#include "component_position.h"
#include "component_graphics.h"
#include "component_text.h"

class SystemGraphics : public System {
	//Pointer to graphics state
	GraphicsState* graphicsState = nullptr;
	vector<AnimationState> states;

	int boxCount = 0;
	BoxData* buffer = nullptr;

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

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, int entityType, int id) override {
		//if buffer is null, dont write yet
		if (!buffer) {
			return;
		}

		//if both position and graphics components are present, this is a simple single state push case 
		if (components.count(typeid(ComponentGraphics)) ) {
			auto pos = getComponent<ComponentPosition>(components);
			auto gra = getComponent<ComponentGraphics>(components);

			bool toDraw;
			auto state = gra->getAnimationState(&toDraw, pos->getPosition3());

			if (toDraw) {
				states.push_back(state);
				buffer[boxCount] = graphicsState->evaluateToBox(state);
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
			vector<BoxData> boxes;
			for (auto i : stateList) {
				boxes.push_back(graphicsState->evaluateToBox(i));
			}
			copy(boxes.begin(), boxes.end(), buffer + boxCount);
			boxCount += boxes.size();
		}
	}

	void precycle(EntityPool* pool) override {
		boxCount = 0;
	}

	void postcycle(EntityPool* pool) override {
		buffer = graphicsState->getBoxDataBuffer(boxCount);
	}

};

#endif