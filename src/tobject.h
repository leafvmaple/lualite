#pragma once
#pragma warning(disable : 4200)

#include <list>

#include "lua.h"
#include "tlimits.h"

typedef double lua_Number;

struct GCObject {
    // GCObject* next;
    lu_byte tt;
    lu_byte marked;
};

struct Table : GCObject {

};

union TString {
    L_Umaxalign dummy;  /* ensures maximum alignment for strings */
    struct : GCObject {
        lu_byte reserved;      // 字符串为系统保留标识符时，这里不为0
        unsigned int hash;
        size_t len;
    } tsv;
    char s[0];
};

struct ClosureHeader : GCObject {
    lu_byte isC;
    lu_byte nupvalues;
    GCObject* gclist;
    Table* env;
};

union Value {
    GCObject* gc;
    void* p;
    lua_Number n;
    int b;
};

struct TValue
{
    Value value;
    int tt;
};

struct Proto : GCObject {
    TValue* k;  /* constants used by the function */
    // Instruction* code;
    struct Proto** p;  /* functions defined inside the function */
};

struct CClosure : ClosureHeader {
    lua_CFunction f;    // 函数指针
    TValue upvalue[0];
};

struct LClosure : ClosureHeader {
    Proto* p;    // 函数原型
    // UpVal* upvals[1];
};


union Closure {
    CClosure c;
    LClosure l;
};

inline void _setnilvalue(TValue* obj) {
    obj->tt = LUA_TNIL;
}

inline void _setpvalue(TValue* obj, void* x) {
    obj->value.p = x;
    obj->tt = LUA_TLIGHTUSERDATA;
}

inline void _sethvalue(TValue* obj, GCObject* x) {
    obj->value.gc = x;
    obj->tt = LUA_TTABLE;
}

inline void _setclvalue(TValue* obj, GCObject* x) {
    obj->value.gc = x;
    obj->tt = LUA_TFUNCTION;
}