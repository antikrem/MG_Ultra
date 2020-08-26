/*Allows the generation of subpooltargets and subpools
The latest optimisation over systems caching*/
#ifndef __SUBPOOLING__
#define __SUBPOOLING__

#include "component.h"
#include "entity_types.h"
#include "entity.h"

#include "algorithm_ex.h"

#include <set>
#include <vector>

// Unpacking struct
template<typename...>
struct Unpacker {};

// A possible target for this subpool
// That is comprised of only components
class SubPoolComponents {
	// This rule requires certain required types
	set<type_index> requiredTypes;

	template<typename T, typename... Args>
	static void addComponent(SubPoolComponents& target, Unpacker<T, Args...>) {
		static_assert(is_base_of<Component, T>::value, "Invalid Component Added: addComponent");
		target.addAComponent(typeid(T));
		addComponent(target, Unpacker<Args...>());
	};

	static void addComponent(SubPoolComponents& target, Unpacker<>) {

	}

public:
	SubPoolComponents(EntityTypes) {

	}

	SubPoolComponents() {

	}

	// Adds a type id to this Subpooling
	void addAComponent(type_index type) {
		requiredTypes.insert(type);
	}

	// SubPoolComponents uses a bunch of components
	template<typename... Args>
	static SubPoolComponents ByComponents() {
		SubPoolComponents target(ETNoType);
		addComponent(target, Unpacker<Args...>());
		return target;
	}

	// Gets set of targets
	set<type_index>& getRequiredTypes() {
		return requiredTypes;
	}

	bool operator==(const SubPoolComponents& rhs) {
		return
			this->requiredTypes == rhs.requiredTypes;
	}

	friend bool operator==(const SubPoolComponents& lhs, const SubPoolComponents& rhs);
};

// The total specification for targeting a SubPool
class SubPoolTarget {
	EntityTypes cachedTarget;
	vector<SubPoolComponents> targets;

public:
	// Empty system
	SubPoolTarget() {
		cachedTarget = ETNoType;
	}

	// This Subpool looks for a specific target
	SubPoolTarget(SubPoolComponents target) {
		cachedTarget = ETNoType;
		targets.push_back(target);
	}

	// Create a subpool with a given targets
	SubPoolTarget(vector<SubPoolComponents> targets) {
		cachedTarget = ETNoType;
		this->targets = targets;
	}

	// This subpool looks for a fixed type
	SubPoolTarget(EntityTypes type) {
		cachedTarget = type;
	}

	void operator=(const SubPoolTarget& rhs) {
		this->cachedTarget = rhs.cachedTarget;
		this->targets = rhs.targets;
	}

	// Checks if this SubPoolTarget is for caching
	bool isCachedTarget() {
		return cachedTarget != ETNoType;
	}

	// Returns the cache type
	EntityTypes getCachedTarget() {
		return cachedTarget;
	}

	// Checks if this SubPoolTarget is a full subpool
	bool isFullSubPool() {
		return cachedTarget == ETNoType;
	}

	// Returns inverted bool value of component targets,
	//used for determining cache only scenarios
	bool isCacheOnly() {
		return !targets.size();
	}

	// Checks if this target is empty
	bool isEmpty() {
		return cachedTarget == ETNoType && !targets.size();
	}

	// Checks if an entity fits in a SubPool
	// only valid if not a cached target
	bool checkEntityInSubPoolTarget(shared_ptr<Entity> ent) {
		//check component map works out
		auto& componentMap = ent->getMapComponent();

		//TODO optimise
		for (auto& target : targets) {
			bool valid = true;
			for (auto& i : target.getRequiredTypes()) {
				if (!componentMap.count(i)) {
					valid = false;
					break;
				}

			}

			if (valid) {
				return true;
			}
		}

		return false;
	}

	friend bool operator==(const SubPoolTarget& lhs, const SubPoolTarget& rhs);
};

// A subpool is kept by the EntityPool
// A subpool can be a facade for a cached entity
// or have the full pool
class SubPool {
	mutex lock;
	SubPoolTarget target;
	vector<shared_ptr<Entity>> ents;
	
public:
	// TODO easy fix
	// Use map<int, SubPool>::find() in pool.h
	SubPool() {
	}

	// Subpool that defers to any entity
	SubPool(SubPoolTarget target) 
		: target(target) {
	}

	// Returns a reference to internal target
	SubPoolTarget& getTarget() {
		return target;
	}

	// Adds an entity, no check
	void addEnt(shared_ptr<Entity> ent) {
		unique_lock<mutex> lck(lock);
		ents.push_back(ent);
	}

	// Adds multiple entities by range
	void addEnts(vector<shared_ptr<Entity>> ents) {
		unique_lock<mutex> lck(lock);
		extend(this->ents, ents);
	}

	// Gets shared ptr from vector, returns nullptr on invalid size
	shared_ptr<Entity> getEntity(int key) {
		unique_lock<mutex> lck(lock);
		return key < (int)ents.size() ? ents[key] : nullptr;
	}

	// Clears the subpool of dead entities
	void clearDeadEnts() {
		unique_lock<mutex> lck(lock);
		erase_sequential_if(ents, [](shared_ptr<Entity> &ent) { return !ent->getFlag(); });
	}

	// Returns size of the ent pool
	int size() {
		unique_lock<mutex> lck(lock);
		return ents.size();
	}

	void operator=(const SubPool& rhs) {
		this->target = rhs.target;
	}
};

#endif