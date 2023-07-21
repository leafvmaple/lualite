#include "lualib.h"
#include "tauxlib.h"

/*
** If your system does not support `stdout', you can just remove this function.
** If you need, you can define your own `print' function, following this
** model but changing `fputs' to put the strings at a proper place
** (a console window or a log file, for instance).
*/
static int luaB_print(lua_State* L) {
    int n = lua_gettop(L);  /* number of arguments */
    for (int i = 1; i <= n; i++) {
        const char* s;
        s = lua_tostring(L, -1);  /* get result */
        if (i > 1) fputs("\t", stdout);
        fputs(s, stdout);
        lua_pop(L, 1);  /* pop result */
    }
    fputs("\n", stdout);
    return 0;
}

static const luaL_Reg base_funcs[] = {
    {"print", luaB_print},
    {nullptr, nullptr}
};

int luaB_cocreate(lua_State* L) {
    return 0;
}

void base_open(lua_State* L) {
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    lua_setglobal(L, "_G");
    luaL_register(L, "_G", base_funcs);

    lua_pushstring(L, LUA_VERSION);
    lua_setglobal(L, "_VERSION");
}

static const luaL_Reg co_funcs[] = {
    { "create", luaB_cocreate },
    { nullptr , nullptr       }
};

int luaopen_base(lua_State* L) {
    base_open(L);
    // luaL_register(L, LUA_COLIBNAME, co_funcs);
    return 1;
}