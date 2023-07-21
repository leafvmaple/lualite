#include "lua.h"
#include "ldo.h"
#include "lparser.h"
#include "lfunc.h"
#include "lvm.h"

// | ci->func | ci->base | ... |        | LUA_MINSTACK - 1 .. | ci->top |
// |          | L->base  | ... | L->top |
int luaD_precall(lua_State* L, TValue* func, int nresults) {
    LClosure* cl = nullptr;
    CallInfo* ci = nullptr;
    int n = 0;

    cl = &func->value.gc->cl.l;
    if (!cl->isC) {
        ci = ++L->ci;
        ci->func = func;
        ci->base = ci->func + 1;
        // TODO
        ci->top = L->top + LUA_MINSTACK;

        L->base = ci->base;
        L->top = ci->top;
        L->savedpc = &cl->p->code.front();

        return PCRLUA;
    }
    else {
        CClosure* c = &func->value.gc->cl.c;
        ci = ++L->ci;
        ci->func = func;
        ci->base = ci->func + 1;
        ci->top = L->top + LUA_MINSTACK;

        L->base = ci->base;
        c->f(L);

        return PCRC;
    }
}

void luaD_call(lua_State* L, TValue* func, int nResults) {
    ++L->nCcalls;
    if (luaD_precall(L, func, nResults) == PCRLUA)
        luaV_execute(L, 1);

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
    cl = luaF_newLclosure(L, 0, &_gt(L)->value.gc->h);
    cl->l.p = tf;
    setclvalue(L->top, cl);
    L->top++;
}

int luaD_protectedparser(lua_State* L, ZIO* z, const char* name) {
    SParser p{z, name};

    f_parser(L, &p);
    return 1;
}