/*An Entity is a collection of Components to be worked on by a System*/
#ifndef __ENTITY__
#define __ENTITY__

#include <map>
#include <typeindex>
#include <memory>
#include <atomic>
#include <mutex>
#include <shared_mutex>

#include "component.h"
#include "entity_types.h"

#include "scriptable_class.h"

/*A colection of components, each entity represents everything discrete in the game
*/
class Entity : public ScriptableClass {
private:
	//lock applied when entity is added to pool
	//blocking component additions
	atomic<bool> inPool = false;
	//map of components
	map<type_index, shared_ptr<Component>> components;
	
	//when false the enemy will be removed from pool
	atomic<bool> flag = true;

	//All entities have a type
	int entityType = ETNoType;
	//set to true when entity is ready to be deleted and memory returned
	bool gcReady = false;

	//function to create an entity in the program stack
	static Entity* createEntity(int type) {
		return new Entity(type);
	}

public:
	Entity() {
		entityType = 0;
	}

	Entity(int type) {
		entityType = type;
	}
	
	//for scrpting purposes
	shared_ptr<Component> l_getComponent(type_index type) {
		return components.count(type) ? components[type] : nullptr;
	}

	shared_ptr<Component> getComponent(type_index type) {
		return components.count(type) ? components[type] : nullptr ;
	}

	//alternative method to get component with template, nullptr on no component
	template <class T>
	shared_ptr<T> getComponent() {
		return components.count(typeid(T)) ? dynamic_pointer_cast<T>(components[typeid(T)]) : nullptr;
	}

	//returns a valid shared pointer if successful, otherwise returns a nullptr
	shared_ptr<Component> addComponent(pair<type_index, Component*> component) {
		if (components.count(component.first) || inPool) {
			return nullptr;
		}
		
		components[component.first] = shared_ptr<Component>(component.second);
		return components[component.first];
	}

	//a lua bindable addComponent
	bool l_addComponent(type_index index, Component* component) {
		if (components.count(index) || inPool) {
			if (inPool) {
				err::logMessage("ENTITY: Error, entity is already in pool (pooled ents cannot have comps added), operation aborted");
			}
			else {
				err::logMessage("ENTITY: Error, entity already has this component, operation aborted");
			}
			return false;
		}
		components[index] = shared_ptr<Component>(component);
		return true;
	}

	//A general update to set entity flag
	void entityUpdate() {
		for (auto i : components) {
			flag = flag && i.second->getFlag();
		}
	}

	//Gets type
	int getType() {
		return entityType;
	}

	//kills entity directly
	void killEntity() {
		flag = false;
	}

	//returns flag, if false, delete this ent
	bool getFlag() {
		return flag;
	}

	//gets inPool flag
	bool isInPool() {
		return inPool;
	}

	//sets this entity to be in the pool
	void markAddToPool() {
		inPool = true;
	}

	//sets the entity to be ready for garbage collection
	void markGCReady() {
		gcReady = true;
	}

	//gets if this entity is ready for reclamation
	bool getGCReady() {
		return gcReady;
	}

	//exposes internal component map for evaluation
	map<type_index, shared_ptr<Component>>& getMapComponent() {
		return components;
	}

	void registerToLua(kaguya::State& state) override {
		state["Entity"].setClass(
			kaguya::UserdataMetatable<Entity>()
			.setConstructors<Entity(int)>()
			.addFunction("getComponent", &Entity::l_getComponent)
			.addFunction("addComponent", &Entity::l_addComponent)
			.addFunction("kill", &Entity::killEntity)
			.addStaticFunction("create", &Entity::createEntity)
		);
	}
};

#endif
