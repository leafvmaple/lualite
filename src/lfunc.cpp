#include "lfunc.h"
#include "lgc.h"

Proto* luaF_newproto(lua_State* L) {
    Proto* p = new Proto;
    luaC_link(L, p, LUA_TPROTO);
    p->k = new std::vector<TValue>;
    p->p = nullptr;
    return p;
}

Closure* luaF_newCclosure(lua_State* L, int nelems, Table* e) {
    Closure* c = (Closure*)malloc(sizeof(CClosure) + sizeof(TValue) * nelems);
    luaC_link(L, &c->c, LUA_TFUNCTION);

    c->c.isC = true;
    c->c.env = e;
    c->c.nupvalues = nelems;

    return c;
}

Closure* luaF_newLclosure(lua_State* L, int nelems, Table* e)
{
    Closure* c = (Closure*)malloc(sizeof(LClosure) + sizeof(TValue) * nelems);
    luaC_link(L, &c->l, LUA_TFUNCTION);

    c->l.isC = false;
    c->l.env = e;
    c->l.nupvalues = nelems;
    while (nelems--) c->l.upvals[nelems] = nullptr;
    return c;
}
