/*Bunch of useful vector stuff*/
#ifndef __VEC_KIT__
#define __VEC_KIT__

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/* All methods are in the vec_kit namespace
 * Handle some vector specific functions
 */
namespace vec_kit {

	// Checks if subset is part of super set
	template <typename T>
	bool isSubset(vector<T> superset, vector<T> subset) {
		if not(superset.size())
			return false;
		sort(subset.begin(), subset.end());
		sort(superset.begin(), superset.end());
		bool inbetween = includes(superset.begin(), superset.end(), subset.begin(), subset.end());
		return inbetween;
	}

	// Cast a vector of type T to a vector of type S
	template<typename T, typename S>
	vector<S> cast(vector<T> source) {
		return vector<S>(source.begin(), source.end());
	}

	// Generates a new vector with the given function and a parameter
	template<typename T, typename P>
	vector<T> generate(int size, const function<T(P)> generator, const P& parameter) {
		vector<T> result;
		for (int i = 0; i < size; i++) {
			result.push_back(generator(parameter));
		}
		return result;
	}

	// Generates a new vector with no additional parameter
	template<typename T, typename P>
	vector<T> generate(int size, const function<T(void)> generator) {
		vector<T> result;
		for (int i = 0; i < size; i++) {
			result.push_back(generator());
		}
		return result;
	}

	// Generates a new vector with a vector of parameters
	template<typename T, typename P>
	vector<T> generate(vector<P> parameters, const function<T(P)> generator) {
		vector<T> result;
		for (auto& p : parameters) {
			result.push_back(generator(p));
		}
		return result;
	}

}

#endif
