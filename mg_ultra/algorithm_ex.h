/* Extends algorthm library
 * Including range based algorithm functions
 */

#ifndef __ALGORITHM_EX__
#define __ALGORITHM_EX__

#include <algorithm>
#include <functional>
#include <type_traits>
#include <stack>
#include <map>
#include <set>
#include <cstdint>

using namespace std;

// Erase if for sequential STL containers
// Removes on true
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

// Erase if for associative STL containers
// Conditional will delete on true
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

// Extends a sequential container with another
template<class Container1, class Container2>
void extend(Container1& container, Container2& extension) {
	container.insert(container.end(), extension.begin(), extension.end());
}

// Checks if container contains element
template <class Container>
bool contains(const Container& container, const typename Container::value_type& element) {
	return std::find(container.begin(), container.end(), element) != container.end();
}

// Finds the index of a given value in a container
// Returns -1 on not finding element
template <class Container>
int index_of(const Container& container, const typename Container::value_type& element) {
	auto it = std::find(container.begin(), container.end(), element);
	return it != container.end() ? distance(container.begin(), it) : -1;
}

// Replaces the value of a container
template <class Container>
void replace(Container& container, const typename Container::value_type& element, const typename Container::value_type& replacement) {
	auto it = std::find(container.begin(), container.end(), element);
	while (it != container.end()) {
		*it = replacement;
		it = std::find(container.begin(), container.end(), element);
	}
}

// Sorts a container
template <class Container>
void sort(Container& container,
		const function<bool(typename Container::value_type&, typename Container::value_type&)> comparator
	) {
	std::sort(container.begin(), container.end(), comparator);
	
}

// Deletes if not null
template <class T>
void delete_or_null(T* &object) {
	if (object) {
		delete object;
		object = nullptr;
	}
}

// Deletes everything inside a stack
// TODO assert type
template <class T>
void clear_stack(T &s) {
	while (!s.empty()) {
		s.pop();
	}
}

// Compares both pointers
// If both are null, returns null
// If one is null, returns other
// If both aren't null returns a
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

// Returns set of keys to a map
template<class T, class D>
set<T> to_keys(map<T, D> container) {
	set<T> result;
	transform(
			container.begin(), 
			container.end(),
			inserter(result, result.end()),
			[](auto pair) 
				{ return pair.first; }
		);
	return result;
}


#endif