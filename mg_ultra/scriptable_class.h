/*Declaractions of static functions for use in lua*/
#ifndef __SCRIPTABLE_CLASS__
#define __SCRIPTABLE_CLASS__

#include "script_unit.h"
#include "global_funcs.h"
#include "error.h"

/*ScriptableClass allows an inherited class to be used in lua
Set derived class as template, overwrite register
additionally, takes another template parameter B, which is a base
finally, the new derived class must have a default constructior
*/
class ScriptableClass {

protected:
	virtual void registerToLua(kaguya::State& state) = 0;
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