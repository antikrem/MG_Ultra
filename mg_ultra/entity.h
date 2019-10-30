/*An Entity is a collection of Components to be worked on by a System*/
#ifndef __ENTITY__
#define __ENTITY__

#include <map>
#include <typeindex>
#include <memory>
#include <mutex>

#include "component.h"
#include "entity_types.h"

using namespace std;

/*A colection of components, each entity represents everything discrete in the game
*/
class Entity {
private:
	map<type_index, shared_ptr<Component>> components;
	//when false the enemy will be cleared
	bool flag = true;
	//All entities have a type
	int entityType = ETNoType;

	//a lock on systemsCaching
	mutex cacheLock;
	//allows for system association with this entity
	//if true, quick evaluate, if false, pass
	map<string, bool> systemsCaching;

public:
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
		if (components.count(component.first)) {
			return nullptr;
		}
		components[component.first] = shared_ptr<Component>(component.second);
		return components[component.first];
	}

	//A general update to set entity flag
	void entityUpdate() {
		for (auto i : components) {
			flag &= i.second->getFlag();
		}
	}

	//Gets type
	int getType() {
		return entityType;
	}

	//returns flag, if false, delete this ent
	bool getFlag() {
		return flag;
	}

	//returns true if this system is part of the caching system
	bool isSystemCached(const string& systemName) {
		unique_lock<mutex> cacheLock;
		return systemsCaching.count(systemName);
	}

	//sets an association 
	void setSystemCache(const string& systemName, bool association) {
		unique_lock<mutex> cacheLock;
		systemsCaching[systemName] = association;
	}

	//returns true if an association exists between the system and this end
	bool isAnAssociatedSystem(const string& systemName) {
		unique_lock<mutex> cacheLock;
		return systemsCaching[systemName];
	}

	//exposes internal component map for evaluation
	map<type_index, shared_ptr<Component>>& getMapComponent() {
		return components;
	}

};

#endif
