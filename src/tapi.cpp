#include "lua.h"
#include "tdo.h"
#include "tfunc.h"
#include "tstate.h"

TValue* index2adr(lua_State* L, int idx) {
    if (idx > 0) {
        return L->base + (idx - 1);
    }
    else if (idx > LUA_REGISTRYINDEX) {
        return L->top + idx;
    }
    else switch (idx) {  /* pseudo-indices */
    case LUA_REGISTRYINDEX: return _registry(L);
    case LUA_ENVIRONINDEX: {
        return nullptr;
    }
    case LUA_GLOBALSINDEX: return _gt(L);
    default: {
        return nullptr;
    }
    }
}

Table* getcurrenv(lua_State* L) {
    if (L->ci == &L->base_ci.front())
        return (Table*)_gt(L)->value.gc;
}

static void f_Ccall(lua_State* L, CCallS* c) {
    Closure* cl;
    cl = luaF_newCclosure(L, 0, getcurrenv(L));
    cl->c.f = c->func;
    _setclvalue(L->top, &cl->c);
    L->top++;
    _setpvalue(L->top, c->ud);
    L->top++;
    luaD_call(L, L->top - 2, 0);
}

void* lua_touserdata(lua_State* L, int idx) {
    TValue* o = index2adr(L, idx);

    switch (o->tt) {
    case LUA_TLIGHTUSERDATA:
        return o->value.p;
    default:
        return nullptr;
    }
}

int lua_cpcall(lua_State* L, lua_CFunction func, void* ud) {
    struct CCallS c;

    c.func = func;
    c.ud = ud;

    return luaD_pcall(L, f_Ccall, &c, _savestack(L, L->top));
}
