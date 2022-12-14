#include "tfunc.h"
#include "tgc.h"

Proto* luaF_newproto(lua_State* L) {
    Proto* p = new Proto;
    luaC_link(L, p, LUA_TPROTO);
    p->k = new std::vector<TValue *>;
    p->code = new std::vector<Instruction>;
    p->lineinfo = new std::vector<int>;
    p->p = nullptr;
    return p;
}

Closure* luaF_newCclosure(lua_State* L, int nelems, Table* e) {
    Closure* c = (Closure*)malloc(sizeof(Closure) + sizeof(TValue) * nelems);
    luaC_link(L, &c->c, LUA_TFUNCTION);

    c->c.isC = true;
    c->c.env = e;
    c->c.nupvalues = nelems;

    return c;
}