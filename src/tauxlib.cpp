#include "tauxlib.h"

int libsize(const luaL_Reg* l) {
	int size = 0;
	for (; l->name; l++) size++;
	return size;
}

const char* luaL_findtable(lua_State* L, int idx, const char* fname, int szhint) {
	lua_pushvalue(L, idx);
	lua_pushstring(L, fname);
	// lua_rawget(L, -2);
	// TODO
	lua_gettable(L, -2);
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
	}

	return nullptr;
}

void luaL_openlib(lua_State* L, const char* libname, const luaL_Reg* l, int nup) {
	luaL_findtable(L, LUA_REGISTRYINDEX, "_LOADED", 1);
	lua_getfield(L, -1, libname);
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, libname);
	}
}

void luaL_register(lua_State* L, const char* libname, const luaL_Reg* l) {
	luaL_openlib(L, libname, l, 0);
}

lua_State* luaL_newstate()
{
    return lua_newstate();
}