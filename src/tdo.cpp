#include "lua.h"
#include "tdo.h"

int luaD_precall(lua_State* L, TValue* func, int nresults)
{
    CClosure* cl = nullptr;
    CallInfo* ci = nullptr;
    int n = 0;

    cl = static_cast<CClosure*>(func->value.gc);
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

void luaD_call(lua_State* L, TValue* func, int nResults)
{
    ++L->nCcalls;

    luaD_precall(L, func, nResults);

    L->nCcalls--;
}

template<typename T>
int luaD_pcall(lua_State* L, void (*func)(lua_State* L, T* ud), T* u, ptrdiff_t oldtop) {
    (*func)(L, u);
    return 0;
}
