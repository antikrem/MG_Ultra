/*Bunch of useful vector stuff*/

#ifndef __VEC_KIT__
#define __VEC_KIT__

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/*All methods are in the vec_kit namespace*/
namespace vec_kit {

	//checks if subset is part of super set
	template <typename T>
	bool isSubset(vector<T> superset, vector<T> subset) {
		if not(superset.size())
			return false;
		sort(subset.begin(), subset.end());
		sort(superset.begin(), superset.end());
		bool inbetween = includes(superset.begin(), superset.end(), subset.begin(), subset.end());
		return inbetween;
	}

	//cast a vector of type T to a vector of type S
	template<typename T, typename S>
	vector<S> cast(vector<T> source) {
		return vector<S>(source.begin(), source.end());
	}
}

#endif
