#pragma once

#include "tstate.h"

struct luaL_Reg {
	const char* name;
	lua_CFunction func;
};

const char* luaL_findtable(lua_State* L, int idx, const char* fname, int szhint);

void luaL_openlib(lua_State* L, const char* libname, const luaL_Reg* l, int nup);
void luaL_register(lua_State* L, const char* libname, const luaL_Reg* l);

lua_State* luaL_newstate();