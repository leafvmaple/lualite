#include "lua.h"
#include "tdo.h"
#include "tfunc.h"
#include "tstate.h"
#include "tstring.h"
#include "tvm.h"

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
    _setclvalue(L->top, cl);
    L->top++;
    _setpvalue(L->top, c->ud);
    L->top++;
    luaD_call(L, L->top - 2, 0);
}

void lua_pushnil(lua_State* L) {
    _setnilvalue(L->top);
    L->top++;
}

void lua_pushlstring(lua_State* L, const char* s, size_t l) {
    _setsvalue(L->top, luaS_newlstr(L, s, l));
    L->top++;
}

void lua_pushstring(lua_State* L, const char* s) {
    if (s == nullptr)
        lua_pushnil(L);
    else
        lua_pushlstring(L, s, strlen(s));
}

void lua_pushvalue(lua_State* L, int idx) {
    _setobj(L->top, index2adr(L, idx));
    L->top++;
}

void lua_pushcclosure(lua_State* L, lua_CFunction fn, int n) {
    Closure* cl;

    cl = luaF_newCclosure(L, n, getcurrenv(L));
    cl->c.f = fn;
    L->top -= n;
    while (n--)
        _setobj(&cl->c.upvalue[n], L->top + n);
    _setclvalue(L->top, cl);
    L->top++;
}

void lua_setfield(lua_State* L, int idx, const char* k) {
    TValue key;
    TValue* t = index2adr(L, idx);;

    _setsvalue(&key, _luaS_new(L, k));
    luaV_settable(L, t, &key, L->top - 1);
    L->top--;
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

void lua_gettable(lua_State* L, int idx) {
    TValue* t = index2adr(L, idx);
    luaV_gettable(L, t, L->top - 1, L->top - 1);
}

void lua_getfield(lua_State* L, int idx, const char* k) {
    TValue key;
    TValue* t = index2adr(L, idx);
    _setsvalue(&key, _luaS_new(L, k));
    luaV_gettable(L, t, &key, L->top);
    L->top++;
}

void lua_settop(lua_State* L, int idx) {
    if (idx >= 0)
        while (L->top < L->base + idx)
            _setnilvalue(L->top++);
    else
        L->top += idx + 1;
}

void lua_pop(lua_State* L, int n) {
    lua_settop(L, -n - 1);
}

bool lua_isnil(lua_State* L, int idx) {
    return lua_type(L, idx) == LUA_TNIL;
}

bool lua_istable(lua_State* L, int idx) {
    return lua_type(L, idx) == LUA_TTABLE;
}

int lua_type(lua_State* L, int idx) {
    TValue* o = index2adr(L, idx);
    return o->tt;
}

void lua_call(lua_State* L, int nargs, int nresults) {
    TValue* func = L->top - (nargs + 1);
    luaD_call(L, func, nresults);
}

int lua_cpcall(lua_State* L, lua_CFunction func, void* ud) {
    struct CCallS c;

    c.func = func;
    c.ud = ud;

    f_Ccall(L, &c);

	// return luaD_pcall(L, f_Ccall, &c, _savestack(L, L->top));
    return 1;

}
