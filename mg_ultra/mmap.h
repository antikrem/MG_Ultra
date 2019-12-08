/*Implementation of multi map*/
#ifndef __M_MAP__
#define __M_MAP__

#include <map>
#include <functional>
#include <vector>
#include <type_traits>

#include "constants.h"

#include "algorithm_ex.h"

using namespace std;

template <class Key, class Value>
class MMap {
	map<Key, vector<Value>> internalMap;

public:
	using iterator = typename map<Key, vector<Value>>::iterator;

	~MMap() {
		clear();
	}

	//counts the number of values indexed by key
	int count(const Key& key) {
		return internalMap.count(key) ? internalMap[key].size() : 0;
	}

	//returns reference to vector values at key
	//if previously no value to key, will make value
	vector<Value>& get(const Key& key) {
		if not(internalMap.count(key)) {
			internalMap[key] = vector<Value>();
		}
		
		return internalMap[key];
	}

	//adds a value to a key
	void add(const Key& key, const Value& value) {
		if (!count(key)) {
			internalMap[key] = vector<Value>();
		}
		internalMap[key].push_back(value);
	}

	//Peels a given key, returning a vector of values
	//and removing that key from map
	//returns empty vector on empty key
	vector<Value> peel(const Key& key) {
		auto it = internalMap.find(key);
		if (it == internalMap.end()) {
			return vector<Value>();
		}
		vector<Value> peeled = it->second;
		internalMap.erase(it);
		return peeled;
	}

	//overlods [] operator
	vector<Value>& operator[](const Key& key) {
		return get(key);
	}

	//returns an iterator to the first element in map
	iterator begin() {
		return internalMap.begin();
	}

	//returns an iterator to the end of the map
	iterator end() {
		return internalMap.end();
	}

	//clear all values from map
	void clear() {
		for (auto& i : internalMap) {
			i.second.clear();
		}
		internalMap.clear();
	}

	//clears all values from the mmap with given function
	void conditionalClear(function<bool(const Value&)> condition) {
		for (auto& i : internalMap) {
			erase_sequential_if(i.second, condition);
		}
	}

	//erases by key
	void erase(const Key& key) {
		internalMap.erase(key);
	}
};

#endif