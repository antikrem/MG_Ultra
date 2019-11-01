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

#include <map>
#include <tuple>

#include "constants.h"

void registerGlobalFunctions(kaguya::State &kaguya);

//using ProfileMap = map<string, tuple<int, int, int>>;
using ProfileMap = map<string, vector<int>>;


ProfileMap getProfileInfoMap();

void setProfileInfoMap(ProfileMap& profileMap);

#endif