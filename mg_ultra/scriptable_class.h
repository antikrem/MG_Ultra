/* Base class for a class to export to lua state for scripting
*/
#ifndef __SCRIPTABLE_CLASS__
#define __SCRIPTABLE_CLASS__

#include "global_funcs.h"
#include "error.h"


template <class T>
static type_index getType() {
	return typeid(T);
}

/* ScriptableClass allows an inherited class to be used in lua
 * Set derived class as template, overwrite register
 * additionally, takes another template parameter B, which is a base
 * finally, the new derived class must have a default constructior
 */
template<class This>
class ScriptableClass {

protected:
	// Required to implement this method to add the class to lua
	// virtual void registerToLua(kaguya::State& state);

	// A templated way to create a new instance of a scriptable class
	// that will remain on the heap
	template<class Temp, class ... Args>
	static Temp* create(Args ... args) {
		return new Temp(args ...);
	}
	/*
	template<class ... Args>
	static This* create(Args ... args) {
		return new This(args ...);
	}
	*/

	/* Forces a class to become scripted
	* The class must have inherited Scriptable class and over written registerToLua
	* The class must also have a default constructor
	*/
	void forceLuaRegistration(kaguya::State& kaguya) {
		ScriptableClass<This>::registerToLua(kaguya);
	}
};




#endif