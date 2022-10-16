#pragma once

#include <vector>
#include <list>
#include <unordered_map>

#include "tobject.h"
#include "ttm.h"

#define LUA_MINSTACK     20
#define BASIC_CI_SIZE    8
#define BASIC_STACK_SIZE (2 * LUA_MINSTACK)

#define EXTRA_STACK   5

struct CallInfo
{
    TValue* top;
    TValue* base;
    TValue* func;
};

struct stringtable {
    // TODO
    std::unordered_map<std::string, GCObject*> hash;
    lu_int32 nuse;
};

struct global_State
{
    stringtable strt;
    lu_byte currentwhite;
    std::list<GCObject*> rootgc;
    lu_mem GCthreshold;
    TValue l_registry;
    TString* tmname[TM_N];  /* array with tag-method names */
};

struct lua_State : GCObject
{
    TValue* top;
    TValue* base;

    std::vector<TValue> stack;
    std::vector<CallInfo> base_ci;

    global_State* l_G;
    CallInfo* ci;

    unsigned short nCcalls;

    TValue l_gt;  /* table of globals */
};

inline TValue* _gt(lua_State* L) {
    return &L->l_gt;
}

inline global_State* &_G(lua_State* L) {
    return L->l_G;
}

inline TValue* _registry(lua_State* L) {
    return &_G(L)->l_registry;
}

lua_State* lua_newstate();

void f_luaopen(lua_State* L);
