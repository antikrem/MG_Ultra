/*Declaractions of static functions for use in lua*/
#ifndef __SCRIPTABLE_CLASS__
#define __SCRIPTABLE_CLASS__

#include "global_funcs.h"
#include "error.h"

template <class T>
static type_index getType() {
	return typeid(T);
}

/*ScriptableClass allows an inherited class to be used in lua
Set derived class as template, overwrite register
additionally, takes another template parameter B, which is a base
finally, the new derived class must have a default constructior
*/
class ScriptableClass {

protected:
	//required to implement this method to add the class to lua
	virtual void registerToLua(kaguya::State& state) = 0;

	//A templated way to create a new instance of a scriptable class
	//that will remain on the heap
	template <class T, class ... Args>
	static T* create(Args ... args) {
		return new T(args ...);
	}
};

/*Forces a class to become scripted
The class must have inherited Scriptable class and over written registerToLua
The class must also have a default constructor*/
template <class T>
void forceLuaRegistration(kaguya::State& kaguya) {

	if (is_base_of<ScriptableClass, T>::value) {
//		T* a = static_cast<T*>(::operator new(sizeof(T)));
//		a->registerToLua(kaguya);
		T temporary;
		temporary.registerToLua(kaguya);
	}
	else {
		string a("A class was forced to bind when it has not inherited ScriptableClass, the class is: ");
		string b(typeid(T).name());
		err::logMessage(a + b);
	}
}


#endif