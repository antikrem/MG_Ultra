/*A component that allows more complex scripting options
Most importantly, allows values to be stored to this component*/
#ifndef __COMPONENT_EXTENDED_SCRIPTING__
#define __COMPONENT_EXTENDED_SCRIPTING__

#include <shared_mutex>
#include "component.h"
#include "scriptable_class.h"
#include "any_type.h"

class ComponentExtendedScripting : public Component, public ScriptableClass {
	map<string, TypeContainer*> reg;

public:
	~ComponentExtendedScripting() {
		for (auto i : reg) {
			delete i.second;
		}
	}

	template <class T>
	void addToBase(string name, T data) {
		if (reg.count(name)) {
			return;
		}
		auto temp = new AnyType<T>(data);
		reg[name] = temp;
	}

	//updates a current value
	template <class T>
	bool update(string name, T data) {
		//check reg contains
		if not(reg.count(name)) {
			return false;
		}

		//check type is correct
		if not(reg[name]->value().hash_code() == typeid(T).hash_code()) {
			return false;
		}

		((AnyType<T>*)reg[name])->setValue(data);
		return true;
	}

	//use valid to check
	template <class T>
	bool get(string name, T *value) {
		//check reg contains
		if not(reg.count(name)) {
			return false;
		}

		//check type is correct
		if not(reg[name]->value().hash_code() == typeid(T).hash_code()) {
			return false;
		}

		//check ptr is legit
		if not(value) {
			return false;
		}

		//cast down and return
		*value = ((AnyType<T>*)reg[name])->pullValue();
		return true;
	}

	//no check get
	//not safe, returns uninitialised values on fail
	template <class T>
	tuple<T, bool> noCheckGet(string name) {
		T val;
		bool valid = get(name, &val);
		return make_tuple(val, valid);
	}

	void registerToLua(kaguya::State& state) override {
		state["ComponentExtendedScripting"].setClass(kaguya::UserdataMetatable<ComponentExtendedScripting, Component>()
			.setConstructors<ComponentExtendedScripting()>()
			.addOverloadedFunctions("add",
				&ComponentExtendedScripting::addToBase<int>,
				&ComponentExtendedScripting::addToBase<float>,
				&ComponentExtendedScripting::addToBase<string>,
				&ComponentExtendedScripting::addToBase<bool>
			)
			.addOverloadedFunctions("set",
				&ComponentExtendedScripting::update<int>,  
				&ComponentExtendedScripting::update<float>,
				&ComponentExtendedScripting::update<string>,
				&ComponentExtendedScripting::update<bool>
			)
			.addOverloadedFunctions("get", 
				&ComponentExtendedScripting::noCheckGet<int>, 
				&ComponentExtendedScripting::noCheckGet<float>, 
				&ComponentExtendedScripting::noCheckGet<string>, 
				&ComponentExtendedScripting::noCheckGet<bool>
			)
			.addStaticFunction("create", ScriptableClass::create<ComponentExtendedScripting>)
			.addStaticFunction("type", &getType<ComponentExtendedScripting>)
			.addStaticFunction("cast", &Component::castDown<ComponentExtendedScripting>)
		);
	}

};

#endif