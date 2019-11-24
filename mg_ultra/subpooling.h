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

//Unpacking struct
template<typename...>
struct Unpacker {};

//A possible target for this subpool
//That is comprised of only components
class SubPoolComponents {
	//This rule requires certain required types
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

	//Adds a type id to this Subpooling
	void addAComponent(type_index type) {
		requiredTypes.insert(type);
	}

	//SubPoolComponents uses a bunch of components
	template<typename... Args>
	static SubPoolComponents ByComponents() {
		SubPoolComponents target(ETNoType);
		addComponent(target, Unpacker<Args...>());
		return target;
	}

	//gets set of targets
	set<type_index>& getRequiredTypes() {
		return requiredTypes;
	}

	bool operator==(const SubPoolComponents& rhs) {
		return
			this->requiredTypes == rhs.requiredTypes;
	}

	friend bool operator==(const SubPoolComponents& lhs, const SubPoolComponents& rhs);
};

//The total specification for targeting a SubPool
class SubPoolTarget {
	EntityTypes cachedTarget;
	vector<SubPoolComponents> targets;

public:

	//create a subpool with a given targets
	SubPoolTarget(vector<SubPoolComponents> targets) {
		cachedTarget = ETNoType;
		this->targets = targets;
	}

	//this Subpool looks for a specific target
	SubPoolTarget(SubPoolComponents target) {
		cachedTarget = ETNoType;
		targets.push_back(target);
	}

	//this subpool looks for a fixed type
	SubPoolTarget(EntityTypes type) {
		cachedTarget = type;
	}

	friend bool operator==(const SubPoolTarget& lhs, const SubPoolTarget& rhs);
};

//A subpool is kept by the EntityPool
//A subpool can be a facade for a cached entity
//or have the full pool
class SubPool {
	mutex lock;
	EntityTypes cachedTarget;
	vector<SubPoolComponents> targets;
	vector<shared_ptr<Entity>> ents;
	
public:
	//subpool that defers to any entity
	SubPool() {
		cachedTarget = ETNoType;
	}

	//this Subpool looks for a specific target
	SubPool(SubPoolComponents target) {
		cachedTarget = ETNoType;
		targets.push_back(target);
	}

	//this subpool looks for anyone of multiple targets
	SubPool(vector<SubPoolComponents> targets) {
		cachedTarget = ETNoType;
		this->targets = targets;
	}

	//this subpool looks for a fixed type
	SubPool(EntityTypes type) {
		cachedTarget = type;
	}

	bool operator==(const SubPool& rhs) { 
		return
			this->cachedTarget == rhs.cachedTarget &&
			this->targets == rhs.targets;
	}

	void operator=(const SubPool& rhs) {
		this->cachedTarget = rhs.cachedTarget;
		this->targets = rhs.targets;
	}

	//checks if this SubPool is for caching
	bool isCachedTarget() {
		return cachedTarget != ETNoType;
	}

	//checks if this SubPool is a full subpool
	bool isFullSubPool() {
		return cachedTarget == ETNoType;
	}

	//Checks if an entity fits in a SubPool
	//only valid if not a cached target
	bool checkEntityInSubPool(shared_ptr<Entity> ent) {
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

	//returns the type of the cached target
	EntityTypes getCachedTarget() {
		return cachedTarget;
	}

	//returns a reference to the internal pool
	//also locks
	vector<shared_ptr<Entity>>& getInternalPool() {
		unique_lock<mutex> lck(lock);
		return ents;
	}

	//Adds an entity, no check
	void addEnt(shared_ptr<Entity> ent) {
		unique_lock<mutex> lck(lock);
		ents.push_back(ent);
	}

	//gets shared ptr from vector, returns nullptr on invalid size
	shared_ptr<Entity> getEntity(int key) {
		unique_lock<mutex> lck(lock);
		return key < (int)ents.size() ? ents[key] : nullptr;
	}

	//clears the subpool of dead entities
	void clearDeadEnts() {
		unique_lock<mutex> lck(lock);
		erase_sequential_if(ents, [](shared_ptr<Entity> &ent) { return !ent->getFlag(); });
	}

	//returns size of the ent pool
	int size() {
		unique_lock<mutex> lck(lock);
		return ents.size();
	}

};

#endif