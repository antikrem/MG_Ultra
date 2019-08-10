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

	int count(Key key) {
		if (map.count(key)) {
			return map[key].size();
		}
		return 0;
	}

	//returns reference to vector values at key
	//if previously no value to key, will make value
	vector<Value>& get(Key key) {
		if not(map.count(key)) {
			map[key] = vector<Value>();

		}
		return map[key];
	}

	//overlods [] operator
	vector<Value>& operator[](Key key) {
		return get(key);
	}

	void clear() {
		for (auto i : map) {
			i.second.clear();
		}
		map.clear();
	}
};

#endif