#include "tfunc.h"
#include "tgc.h"

Closure* luaF_newCclosure(lua_State* L, int nelems, Table* e) {
    Closure* c = (Closure*)malloc(sizeof(Closure) + sizeof(TValue) * nelems);
    luaC_link(L, &c->c, LUA_TFUNCTION);

    c->c.isC = true;
    c->c.env = e;
    c->c.nupvalues = nelems;

    return c;
}