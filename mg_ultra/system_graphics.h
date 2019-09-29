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
	//vao to write to
	Vao* vao = nullptr;

public:
	SystemGraphics() {
		debugName = "s_graphics";
		types.push_back(typeid(ComponentPosition));
		types.push_back(typeid(ComponentGraphics));
		types.push_back(typeid(ComponentText));
	}

	void setGraphicsState(GraphicsState* graphicsState) {
		this->graphicsState = graphicsState;
	}

	void handleComponentMap(map<type_index, Component*> components, int entityType, int id) override {
		//if both position and graphics components are present, this is a simple single state push case 
		if ( components.count(typeid(ComponentPosition)) && components.count(typeid(ComponentGraphics)) ) {
			auto pos = (ComponentPosition*)components[typeid(ComponentPosition)];
			auto gra = (ComponentGraphics*)components[typeid(ComponentGraphics)];

			bool toDraw;
			auto state = gra->getAnimationState(&toDraw, pos->getPosition3());

			if (toDraw) {
				states.push_back(state);
			}
		}
		//if otherwise a text component is found, get cached frames
		else if ( components.count(typeid(ComponentPosition)) && components.count(typeid(ComponentText)) ) {
			auto pos = (ComponentPosition*)components[typeid(ComponentPosition)];
			auto tex = (ComponentText*)components[typeid(ComponentText)];
			//exit early if not visible
			if not(tex->getVisible()) {
				return;
			}

			//pull all the states 
			auto stateList = tex->getStates(pos->getPosition3());
			states.insert(states.end(), stateList.begin(), stateList.end());
		}
	}

	void precycle(EntityPool* pool) override {
		//clear all states
		states.clear();
		//get a vao
		vao = graphicsState->getLowestVao();
		//sometimes this will be null, return in this case
		if not(vao) {
			return;
		}
		//request a memory space to write to
		vao->requestMapAsync();
	}

	void postcycle(EntityPool* pool) override {
		//graphics state hasn't been set yet, so no render
		if not(vao) {
			return;
		}
		//otherwise, get the memory to map to
		BoxData* boxMap = vao->returnMapAsync();
		//can be null, return in this case
		if not(boxMap) {
			return;
		}

		//todo make this a direct write to unmapped memory, also check mem size
		BoxData data[1000];
		unsigned int boxcount = states.size();

		for (unsigned int i = 0; i < boxcount; i++) {
			data[i] = graphicsState->evaluateToBox(states[i]);
		}

		memcpy(boxMap, data, boxcount * sizeof(BoxData));
		vao->requestUnmap(boxcount, graphicsState->getNextUnmap());

	}

};

#endif