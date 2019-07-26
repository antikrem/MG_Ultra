#include "pool.h"

EntityPool* lastPool = nullptr;

EntityPool::EntityPool() {
	lastPool = this;
}

EntityPool* getLastPool() {
	return lastPool;
}