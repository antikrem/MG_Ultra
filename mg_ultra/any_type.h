#ifndef __ANY_TYPE__
#define __ANY_TYPE__

#include <typeinfo>
#include <typeindex>

using namespace std;

class TypeContainer {
protected:
	type_index type = typeid(void);
public:

	virtual ~TypeContainer() {

	}

	template <class T>
	void initialise() {
		type = typeid(T);
	}

	type_index value() {
		return type;
	}
};

/*Anytype copiable variable*/
template<class T>
class AnyType : public TypeContainer {
	T data;
public:
	AnyType(T ini) {
		data = ini;
		initialise<T>();
	}

	T pullValue() {
		return data;
	}

	type_index getInfo() {
		return type;
	}

	void setValue(T data) {
		this->data = data;
	}
};

#endif