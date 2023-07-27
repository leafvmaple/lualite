#include "lua.h"
#include "ldo.h"
#include "lfunc.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"
#include "lzio.h"

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

// ��ȡ��ǰ���ڻ�����
Table* getcurrenv(lua_State* L) {
    if (L->ci == &L->base_ci.front())
        return (Table*)_gt(L)->value.gc;
}

// ��C����ѹջ������
// |         |           |
// | Closure | UsderData | L->top
static void f_Ccall(lua_State* L, CCallS* c) {
    CClosure* cl;
    cl = luaF_newCclosure(L, 0, getcurrenv(L));
    cl->f = c->func;
    setclvalue(L->top++, cl, "#[f_Ccall] CClosure#");
    setpvalue(L->top++, c->ud, "#[f_Ccall] ud#");
    luaD_call(L, L->top - 2, 0);
}

void lua_pushnil(lua_State* L) {
    setnilvalue(L->top);
    L->top++;
}

void lua_pushlstring(lua_State* L, const char* s, size_t l _IMPL) {
    setsvalue(L->top++, luaS_newlstr(L, s, l), debug ? debug : s);
}

void lua_pushstring(lua_State* L, const char* s _IMPL) {
    if (s == nullptr)
        lua_pushnil(L);
    else
        lua_pushlstring(L, s, strlen(s), debug);
}

void lua_pushvalue(lua_State* L, int idx) {
    setobj(L->top++, index2adr(L, idx));
}

void lua_pushcclosure(lua_State* L, lua_CFunction fn, int n _IMPL) {
    CClosure* cl;

    cl = luaF_newCclosure(L, n, getcurrenv(L));
    cl->f = fn;
    L->top -= n;
    while (n--)
        setobj(&cl->upvalue[n], L->top + n);
    setclvalue(L->top, cl, debug);
    L->top++;
}

void lua_pushcfunction(lua_State* L, lua_CFunction f _IMPL) {
    lua_pushcclosure(L, f, 0, debug);
}

void lua_settable(lua_State* L, int idx) {
    TValue* t = index2adr(L, idx);
    luaV_settable(L, t, L->top - 2, L->top - 2);
    L->top -= 2;
}

void lua_setfield(lua_State* L, int idx, const char* k) {
    TValue key;
    TValue* t = index2adr(L, idx);;

    setsvalue(&key, luaS_new(L, k), k);
    luaV_settable(L, t, &key, L->top - 1);
    L->top--;
}

void lua_createtable(lua_State* L, int narr, int nrec _IMPL) {
    sethvalue(L->top, luaH_new(L, narr, nrec), debug);
    L->top++;
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
    setsvalue(&key, luaS_new(L, k));
    luaV_gettable(L, t, &key, L->top);
    L->top++;
}

void lua_getglobal(lua_State* L, int idx, const char* k) {
    lua_getfield(L, LUA_GLOBALSINDEX, k);
}

int lua_gettop(lua_State* L) {
    return static_cast<int>(L->top - L->base);
}

void lua_settop(lua_State* L, int idx) {
    if (idx >= 0)
        while (L->top < L->base + idx)
            setnilvalue(L->top++);
    else
        L->top += idx + 1;
}

void lua_pop(lua_State* L, int n) {
    lua_settop(L, -n - 1);
}

void lua_remove(lua_State* L, int idx) {
    TValue* p = index2adr(L, idx);
    while (++p < L->top) setobj(p - 1, p);
    L->top--;
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

int lua_load(lua_State* L, lua_Reader reader, void* data, const char* chunkname) {
    ZIO z{0, nullptr, reader, data, L};
    return luaD_protectedparser(L, &z, chunkname);
}

const char* lua_tolstring(lua_State* L, int idx, size_t* len){
    TValue* o = index2adr(L, idx);
    TString* ts = static_cast<TString*>(o->value.gc);
    if (len)
        *len = ts->len;
    return ts->s;
}

const char* lua_tostring(lua_State* L, int idx) {
    return lua_tolstring(L, idx, nullptr);
}