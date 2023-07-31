#pragma once

#include <vector>
#include <list>
#include <unordered_map>

#include "lobject.h"
#include "ltm.h"

#define LUA_MINSTACK     20
#define BASIC_CI_SIZE    8
#define BASIC_STACK_SIZE (2 * LUA_MINSTACK)

#define EXTRA_STACK   5

struct CallInfo
{
    TValue* top  = nullptr;
    TValue* base = nullptr;
    TValue* func = nullptr;

    int nresults = 0; // ����������ý��ܶ��ٸ�����ֵ

    const Instruction* savedpc = nullptr;
};

struct stringtable {
    // TODO
    std::unordered_map<std::string, GCheader*> hash;
    lu_int32 nuse;
};

struct global_State
{
    stringtable strt;
    lua_marked currentwhite;
    std::list<GCheader*> rootgc;
    lu_mem GCthreshold = 0;
    TValue l_registry;
    TString* tmname[TM_N];  /* array with tag-method names */
};

struct lua_State : GCheader
{
    TValue* top  = nullptr;
    TValue* base = nullptr;

    std::vector<TValue> stack;
    std::vector<CallInfo> base_ci;

    global_State* l_G          = nullptr;
    CallInfo* ci               = nullptr;
    const Instruction* savedpc = nullptr; // ��ǰ��������ʼָ��

    unsigned short nCcalls     = 0;

    TValue l_gt;  /* table of globals */
    TValue env;
};

// ��ȡȫ�ֱ�����
inline TValue* _gt(lua_State* L) {
    return &L->l_gt;
}

// ��ȡȫ��״̬��
inline global_State* &G(lua_State* L) {
    return L->l_G;
}

inline TValue* _registry(lua_State* L) {
    return &G(L)->l_registry;
}

void f_luaopen(lua_State* L);
