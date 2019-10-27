#include "pool.h"

EntityPool* globalPool = nullptr;

EntityPool* getGlobalPool() {
	return globalPool;
}

void setGlobalPool(EntityPool* pool) {
	globalPool = pool;
}