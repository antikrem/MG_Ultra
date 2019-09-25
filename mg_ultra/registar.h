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

/*Singleton class, allows systems to exchange states
save only for global values
Exposed to lua*/
class Registar {
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

	//no check get
	//not safe, returns uninitialised values on fail
	template <class T>
	tuple<T, bool> noCheckGet(string name) {
		T val;
		bool valid = get(name, &val);
		return make_tuple(val, valid);
	}

};

namespace g_registar {
	
	void setGlobalRegistar(Registar* registar);

	Registar* getGlobalRegistar();

}

#endif