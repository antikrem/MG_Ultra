/*System interaction*/
#ifndef __REGISTAR__
#define __REGISTAR__

#include <map>
#include <string>

#include <shared_mutex>
#include <tuple>

#include "constants.h"
#include "error.h"

#include "any_type.h"

#include "scriptable_class.h"

/*Singleton class, allows systems to exchange states
save only for global values
Exposed to lua*/
class Registar : public ScriptableClass {
	map<string, TypeContainer*> reg;
	shared_mutex lock;

public:
	~Registar() {
		for (auto i : reg) {
			delete i.second;
		}
	}

	template <class T>
	void addToBase(string name, T data) {
		unique_lock<shared_mutex> lck(lock);
		//todo check if reg has same things
		auto temp = new AnyType<T>(data);
		reg[name] = temp;
	}

	//add to base for ints
	template <>
	void addToBase<int>(string name, int data) {
		unique_lock<shared_mutex> lck(lock);
		//todo check if reg has same things
		auto temp = new AnyType<float>((float)data);
		reg[name] = temp;
	}

	//updates a current value
	template <class T>
	bool update(string name, T data) {
		{
			shared_lock<shared_mutex> lck(lock);
			//check reg contains
			if not(reg.count(name)) {
				return false;
			}

			//check type is correct
			if not(reg[name]->value().hash_code() == typeid(T).hash_code()) {
				return false;
			}
		}
		
		unique_lock<shared_mutex> lck(lock);
		((AnyType<T>*)reg[name])->setValue(data);
		return true;
	}

	//updates a current float value
	template <>
	bool update<int>(string name, int data) {
		{
			shared_lock<shared_mutex> lck(lock);
			//check reg contains
			if not(reg.count(name)) {
				return false;
			}

			//check type is correct
			if not(reg[name]->value().hash_code() == typeid(float).hash_code()) {
				return false;
			}
		}

		unique_lock<shared_mutex> lck(lock);
		((AnyType<float>*)reg[name])->setValue((float)data);
		return true;
	}

	//use valid to check
	template <class T>
	bool get(string name, T *value) {
		shared_lock<shared_mutex> lck(lock);
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

	//use valid to check, specialised for ints
	template <>
	bool get<int>(string name, int *value) {
		shared_lock<shared_mutex> lck(lock);
		//check reg contains
		if not(reg.count(name)) {
			return false;
		}

		//check type is correct
		if not(reg[name]->value().hash_code() == typeid(float).hash_code()) {
			return false;
		}

		//check ptr is legit
		if not(value) {
			return false;
		}

		//cast down and return
		*value = (int)((AnyType<float>*)reg[name])->pullValue();
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
		state["Registar"].setClass(
			kaguya::UserdataMetatable<Registar>()
			.setConstructors<Registar()>()
			.addFunction("getInt", &Registar::noCheckGet<int>)
			.addFunction("getFloat", &Registar::noCheckGet<float>)
			.addFunction("getString", &Registar::noCheckGet<string>)
			.addFunction("getBool", &Registar::noCheckGet<bool>)
			.addOverloadedFunctions("add", &Registar::addToBase<int>, &Registar::addToBase<float>, &Registar::addToBase<string>, &Registar::addToBase<bool>)
			.addOverloadedFunctions("update", &Registar::update<int>, &Registar::update<float>, &Registar::update<string>, &Registar::update<bool>)
		);
	}
};

namespace g_registar {
	
	void setGlobalRegistar(Registar* registar);

	Registar* getGlobalRegistar();

}

#endif