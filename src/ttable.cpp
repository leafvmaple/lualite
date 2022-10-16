#include "ttable.h"

#include "tobject.h"
#include "tgc.h"

Table* luaH_new(lua_State* L, int narray, int nhash)
{
    Table* t = new Table;
    luaC_link(L, t, LUA_TTABLE);

    return t;
}
