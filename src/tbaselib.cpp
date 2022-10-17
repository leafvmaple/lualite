#include "lualib.h"
#include "tauxlib.h"

int luaB_cocreate(lua_State* L) {
	return 0;
}

void base_open(lua_State* L) {
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	lua_setglobal(L, "_G");

	lua_pushstring(L, LUA_VERSION);
	lua_setglobal(L, "_VERSION");
}

static const luaL_Reg co_funcs[] = {
    { "create", luaB_cocreate },
    { nullptr , nullptr       }
};

int luaopen_base(lua_State* L) {
	base_open(L);
	luaL_register(L, LUA_COLIBNAME, co_funcs);
	return 1;
}