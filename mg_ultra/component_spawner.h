/*provides an interface for an entity to be created
then pushed to the pool*/
#ifndef __COMPONENT_SPAWNER__
#define __COMPONENT_SPAWNER__

#include "component.h"
#include "scriptable_class.h"
#include <mutex>
#include <vector>

//forward declaraction that entities exist
class Entity;

class ComponentSpawner : public Component, public ScriptableClass<ComponentSpawner> {
	mutex entitiesLock;
	vector<Entity*> entities;

	mutex factoryLock;
	Entity* factoryEntity = nullptr;

public:
	//destroys this component
	~ComponentSpawner() {
		for (auto i : entities) {
			delete i;
		}
		if (factoryEntity) {
			delete factoryEntity;
		}
	}

	//create an entity by factory
	//returns nullptr if an error occured
	Entity* createFactoryEntity(unsigned int type) {
		unique_lock<mutex>(factoryLock);
		if (factoryEntity) {
			err::logMessage("SPAWNER ERROR: An entity was requested of type: " + to_string(type) + " when one was already being built");
			return nullptr;
		}
		else {
			factoryEntity = new Entity(type);
			return factoryEntity;
		}
	}

	//adds a component to the current factory
	void addComponentToEntity(Component* component) {
		unique_lock<mutex>(factoryLock);
		factoryEntity->addComponent(component->pullForEntity());

	}

	void pushFactoryEntity() {
		unique_lock<mutex>(entitiesLock);
		unique_lock<mutex>(factoryLock);
		if (!factoryEntity) {
			err::logMessage("SPAWNER ERROR: An entity was pushed when a factory entity does not exist");
			return;
		}
		entities.push_back(factoryEntity);
		factoryEntity = nullptr;

	}

	void addEntity(Entity* entity) {
		unique_lock<mutex>(entitiesLock);
		entities.push_back(entity);
	}

	void pullEnts(vector<Entity*>& ents) {
		unique_lock<mutex>(entitiesLock);
		unique_lock<mutex>(factoryLock);
		ents = entities;
		entities.clear();
	}

	static void registerToLua(kaguya::State& state) {
		state["ComponentSpawner"].setClass(kaguya::UserdataMetatable<ComponentSpawner, Component>()
			.setConstructors<ComponentSpawner()>()
			.addFunction("create_entity", &ComponentSpawner::createFactoryEntity)
			.addFunction("add_component", &ComponentSpawner::addComponentToEntity)
			.addFunction("push_entity", &ComponentSpawner::pushFactoryEntity)
			.addFunction("add_entity", &ComponentSpawner::addEntity)
			.addStaticFunction("create", ScriptableClass::create<ComponentSpawner>)
			.addStaticFunction("type", &getType<ComponentSpawner>)
			.addStaticFunction("cast", &Component::castDown<ComponentSpawner>)
		);
	}

};

#endif