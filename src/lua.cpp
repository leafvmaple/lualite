#include "lua.h"
#include "tauxlib.h"

struct Smain {
    int argc;
    char** argv;
    int status;
};

static int pmain(lua_State* L) {
    Smain* s = (struct Smain*)lua_touserdata(L, 1);
    return 0;
}

int main(int argc, char** argv) {
    int ret = 0;
    struct Smain s;

    s.argc = argc;
    s.argv = argv;

    lua_State* L = luaL_newstate();
    ret = lua_cpcall(L, &pmain, &s);
}