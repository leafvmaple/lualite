#include "lfunc.h"
#include "lgc.h"

Proto* luaF_newproto(lua_State* L) {
    Proto* p = new Proto;
    luaC_link(L, p, LUA_TPROTO);
    p->p = nullptr;
    return p;
}

CClosure* luaF_newCclosure(lua_State* L, int nelems, Table* e) {
    CClosure* c = (CClosure*)malloc(sizeof(CClosure) + sizeof(TValue) * nelems);
    luaC_link(L, c, LUA_TFUNCTION);

    c->isC = true;
    c->env = e;
    c->nupvalues = nelems;

    return c;
}

LClosure* luaF_newLclosure(lua_State* L, int nelems, Table* e)
{
    LClosure* l = (LClosure*)malloc(sizeof(LClosure) + sizeof(TValue) * nelems);
    luaC_link(L, l, LUA_TFUNCTION);

    l->isC = false;
    l->env = e;
    l->nupvalues = nelems;
    while (nelems--) l->upvals[nelems] = nullptr;
    return l;
}
