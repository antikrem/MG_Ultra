/*A Component is an aspect of an Entity*/
#ifndef __COMPONENT_STATE__
#define __COMPONENT_STATE__

#include <utility>
#include <typeindex>
#include <atomic>
#include <memory>

using namespace std;

//Components are not thread safe per se,
//Each derivation will need to be implemented such that acccess is safe 
class Component {
private:
	//Each component has its own flag
	atomic<bool> flag = true;
public:

	//use to kill a entity
	void killEntity() {
		flag.store(false);
	}

	bool getFlag() {
		return flag.load();
	}

	virtual pair<type_index, Component*> pullForEntity() {
		return make_pair<type_index, Component*>(typeid(*this), this);
	}

	//Allows a component to cast itself into a derived type
	template <class T> static shared_ptr<T> castDown(shared_ptr<Component> in) {
		return dynamic_pointer_cast<T>(in);
	}

};


#endif