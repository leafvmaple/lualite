#include "lua.h"
#include "tdo.h"
#include "tparser.h"


int luaD_precall(lua_State* L, TValue* func, int nresults) {
    CClosure* cl = nullptr;
    CallInfo* ci = nullptr;
    int n = 0;

    cl = (CClosure*)func->value.gc;
    if (cl->isC)
    {
        ci = ++L->ci;
        ci->func = func;
        ci->base = ci->func + 1;
        ci->top = L->top + LUA_MINSTACK;

        L->base = ci->base;
        cl->f(L);
    }

    return PCRC;
}

void luaD_call(lua_State* L, TValue* func, int nResults) {
    ++L->nCcalls;
    luaD_precall(L, func, nResults);
    L->nCcalls--;
}

struct SParser {  /* data to `f_parser' */
	ZIO* z;
	const char* name;
};

void f_parser(lua_State* L, SParser* p) {
    Proto* tf = nullptr;
    Closure* cl = nullptr;

    tf = luaY_parser(L, p->z, p->name);
}

int luaD_protectedparser(lua_State* L, ZIO* z, const char* name) {
    SParser p;
    p.z = z;
    p.name = name;

    f_parser(L, &p);
    return 1;
}