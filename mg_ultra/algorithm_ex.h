/*Extends algorthm library
Including range based algorithm functions*/

#ifndef __ALGORITHM_EX__
#define __ALGORITHM_EX__

#include <algorithm>
#include <functional>
#include <type_traits>
#include <stack>
#include <cstdint>

using namespace std;

//erase if for sequential STL containers
template <class Container>
void erase_sequential_if(Container& container, const function<bool(typename Container::value_type&)> conditional) {
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
//conditional will delete on true
template <class Container>
void erase_associative_if(Container& container, const function<bool(typename Container::value_type&)> conditional) {
	//use a while loop
	auto it = container.begin();
	while (it != container.end()) {
		if (conditional(*it)) {
			it = container.erase(it);
		}
		else {
			it++;
		}
	}
}

//Checks if container contains element
template <class Container>
bool contains(const Container& container, const typename Container::value_type& element) {
	return std::find(container.begin(), container.end(), element) != container.end();
}

//deletes if not null
template <class T>
void delete_or_null(T* &object) {
	if (object) {
		delete object;
		object = nullptr;
	}
}

//deletes everything inside a stack
//TODO assert type
template <class T>
void clear_stack(T &s) {
	while (!s.empty()) {
		s.pop();
	}
}

//compares both pointers
//if both are null, returns null
//if one is null, returns other
//if both aren't null returns a
template <class T>
T* ptr_or(T* a, T* b) {
	if (a && b) {
		return a;
	}
	return (T*)(
		reinterpret_cast<std::uintptr_t>(a)
		| reinterpret_cast<std::uintptr_t>(b)
	);
}

#endif