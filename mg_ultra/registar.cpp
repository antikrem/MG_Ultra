#include "registar.h"

Registar* globalRegistar = nullptr;

void g_registar::setGlobalRegistar(Registar* registar) {
	globalRegistar = registar;
}

Registar* g_registar::getGlobalRegistar() {
	return globalRegistar;
}