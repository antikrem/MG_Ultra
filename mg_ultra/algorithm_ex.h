/*Extends algorthm library
Including range based algorithm functions*/

#ifndef __ALGORITHM_EX__
#define __ALGORITHM_EX__

#include <algorithm>
#include <functional>
#include <type_traits>

using namespace std;

//erase if for sequential STL containers
template <class Container>
void erase_sequential_if(Container &container, function<bool(typename Container::value_type&)> conditional) {
	//utilise erase/remove_if
	container.erase(
		remove_if(
			container.begin(),
			container.end(),
			conditional
		),
		container.end()
	);
}

//erase if for associative STL containers
template <class Container>
void erase_associative_if(Container &container, function<bool(typename Container::value_type&)> conditional) {
	//use a while loop
	auto it = container.begin();
	while (it != container.end()) {
		if (conditional(*it)) {
			it++;
		}
		else {
			it = container.erase(it);
		}
	}
}

#endif