/*Declaractions of static functions for use in lua*/
#ifndef __GLOBAL_FUNC__
#define __GLOBAL_FUNC__

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#pragma comment(lib, "liblua53.a")

#include "kaguya.hpp"

#include "error.h"

void registerGlobalFunctions(kaguya::State &kaguya);

#endif