/*Implementation of multi map*/
#ifndef __M_MAP__
#define __M_MAP__

#include <map>
#include <vector>

#include "constants.h"

using namespace std;

template <class Key, class Value>
class MMap {
	map<Key, vector<Value>> map;

public:
	~MMap() {
		clear();
	}

	//counts the number of values indexed by key
	int count(const Key& key) {
		return map.count(key) ? map[key].size() : 0;
	}

	//returns reference to vector values at key
	//if previously no value to key, will make value
	vector<Value>& get(const Key& key) {
		if not(map.count(key)) {
			map[key] = vector<Value>();
		}
		return map[key];
	}

	//adds a value to a key
	void add(const Key& key, const Value& value) {
		if (!count(key)) {
			map[key] = vector<Value>();
		}
		map[key].push_back(value);
	}

	//Peels a given key, returning a vector of values
	//and removing that key from map
	//returns empty vector on empty key
	vector<Value> peel(const Key& key) {
		auto it = map.find(key);
		if (it == map.end()) {
			return vector<Value>();
		}
		vector<Value> peeled = it->second;
		map.erase(it);
		return peeled;
	}

	//overlods [] operator
	vector<Value>& operator[](const Key& key) {
		return get(key);
	}

	//clear all values from map
	void clear() {
		for (auto i : map) {
			i.second.clear();
		}
		map.clear();
	}
};

#endif