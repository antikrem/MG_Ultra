/*An Entity is a collection of Components to be worked on by a System*/
#ifndef __ENTITY__
#define __ENTITY__

#include <map>
#include <typeindex>
#include <typeinfo>
#include <memory>
#include <atomic>
#include <mutex>
#include <shared_mutex>

#include "algorithm_ex.h"
#include "component.h"
#include "entity_types.h"

#include "scriptable_class.h"

/*A colection of components, each entity represents everything discrete in the game
*/
class Entity : public ScriptableClass<Entity> {
private:

	// Lock applied when entity is added to pool
	// Blocking component additions
	atomic<bool> inPool = false;

	// Map of components
	map<type_index, shared_ptr<Component>> components;
	
	// When false the enemy will be removed from pool
	atomic<bool> flag = true;

	// Set to false when internal cleanup has occured
	atomic<bool> componentCleanUpRequired = true;

	// All entities have a type
	int entityType = ETNoType;

	// Set to true when entity is ready to be deleted and memory returned
	bool gcReady = false;

	// Function to create an entity in the program stack
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

	// Returns set of component types
	set<type_index> getComponentSignature() {
		return to_keys(components);
	}
	
	// For scrpting purposes
	shared_ptr<Component> l_getComponent(type_index type) {
		return components.count(type) ? components[type] : nullptr;
	}

	shared_ptr<Component> getComponent(type_index type) {
		return components.count(type) ? components[type] : nullptr ;
	}

	// Alternative method to get component with template, nullptr on no component
	template <class T>
	shared_ptr<T> getComponent() {
		return components.count(typeid(T)) ? dynamic_pointer_cast<T>(components[typeid(T)]) : nullptr;
	}

	// Returns a valid shared pointer if successful, otherwise returns a nullptr
	shared_ptr<Component> addComponent(pair<type_index, Component*> component) {
		if (components.count(component.first) || inPool) {
			return nullptr;
		}
		
		components[component.first] = shared_ptr<Component>(component.second);
		return components[component.first];
	}

	// Optimised form of addComponent when component type is known
	template <class T>
	shared_ptr<Component> addComponent(T* component) {
		static_assert(std::is_base_of<Component, T>::value, "entity.h, failed addComponent base_of check");

		type_index ti = typeid(T);
		if (components.count(ti) || inPool) {
			return nullptr;
		}

		components[ti] = shared_ptr<Component>(component);
		return components[ti];
	}

	// A lua bindable addComponent
	bool l_addComponent(type_index index, Component* component) {
		if (components.count(index) || inPool) {
			if (inPool) {
				err::logMessage("ENTITY: Error, entity is already in pool (pooled ents cannot have comps added), operation aborted");
			}
			else {
				string compName = (typeid(*component)).name();
				err::logMessage("ENTITY: Error, entity already has this component: " + compName + ", operation aborted");
			}
			return false;
		}
		components[index] = shared_ptr<Component>(component);
		return true;
	}

	// A general update to set entity flag
	void entityUpdate() {
		bool tempFlag = true;
		for (auto i : components) {
			tempFlag = tempFlag && i.second->getFlag();
		}
		if (!tempFlag) {
			killEntity();
		}
	}

	// Gets type
	int getType() {
		return entityType;
	}

	// Kills entity directly
	void killEntity();

	// Clean up when entity gets to graveyard
	void cleanup();

	// Returns flag, if false, delete this ent
	bool getFlag() {
		return flag;
	}

	// Gets inPool flag
	bool isInPool() {
		return inPool;
	}

	// Sets this entity to be in the pool
	void markAddToPool() {
		inPool = true;
	}

	// Sets the entity to be ready for garbage collection
	void markGCReady() {
		gcReady = true;
	}

	// Gets if this entity is ready for reclamation
	bool getGCReady() {
		return gcReady;
	}

	// Exposes internal component map for evaluation
	map<type_index, shared_ptr<Component>>& getMapComponent() {
		return components;
	}

	static void registerToLua(kaguya::State& state) {
		state["Entity"].setClass(
			kaguya::UserdataMetatable<Entity>()
			.setConstructors<Entity(int)>()
			.addFunction("getComponent", &Entity::l_getComponent)
			.addFunction("addComponent", &Entity::l_addComponent)
			.addFunction("kill", &Entity::killEntity)
			.addFunction("is_alive", &Entity::getFlag)
			.addStaticFunction("create", &Entity::createEntity)
		);
	}
};

#endif