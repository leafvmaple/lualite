#pragma once
#pragma warning(disable : 4200)

#include <list>
#include <vector>
#include <unordered_map>

#include "lua.h"
#include "tlimits.h"

#define LAST_TAG        LUA_TTHREAD

#define NUM_TAGS        (LAST_TAG + 1)

#define LUA_TPROTO      (LAST_TAG + 1)
#define LUA_TUPVAL      (LAST_TAG + 2)
#define LUA_TDEADKEY    (LAST_TAG + 3)

typedef double lua_Number;

struct TValue;
union GCObject;

struct GCheader {
    // GCObject* next;
    lu_byte tt;
    lu_byte marked;
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

    bool operator==(const TValue& k) const;
};

union TString {
    L_Umaxalign dummy;  /* ensures maximum alignment for strings */
    struct : GCheader {
        lu_byte reserved;      // 字符串为系统保留标识符时，这里不为0
        unsigned int hash;
        size_t len;
        char s[0];
    } tsv;

};

inline size_t keyhash(const TValue& k) {
    switch (k.tt)
    {
    case LUA_TSTRING:
    {
        TString* s = (TString*)k.value.gc;
        return std::hash<const char*>{}(s->tsv.s);
    }
    default:
    {
        return 0;
    }
    }
}

inline bool TValue::operator==(const TValue& k) const {
    return keyhash(*this) == keyhash(k);
}

struct KeyFunction {
    size_t operator()(const TValue& k) const {
        return keyhash(k);
    }
};


struct Table : GCheader {
    Table* metatable;
    std::vector<TValue>* array;
    std::unordered_map<const TValue, TValue, KeyFunction>* node;
};


struct ClosureHeader : GCheader {
    lu_byte isC;
    lu_byte nupvalues;
    GCObject* gclist;
    Table* env;
};

struct Proto : GCheader {
    std::vector<TValue*>* k; /* 被该函数引用到的常量 */
    Instruction* code;
    struct Proto** p;  /* 函数内嵌套函数 */
};

// C函数中的指令和数据都在代码段数据段中，只需要一个函数指针入口即可
struct CClosure : ClosureHeader {
    lua_CFunction f;    // 函数指针
    TValue upvalue[0];
};

// Lua函数的指令，数据需要自行管理，因此需要一个Proto来存储
struct LClosure : ClosureHeader {
    Proto* p;    // 函数原型
    // UpVal* upvals[1];
};


union Closure {
    CClosure c;
    LClosure l;
};

bool ttisnumber(TValue* obj);

inline void setnilvalue(TValue* obj) {
    obj->tt = LUA_TNIL;
}

inline void setnvalue(TValue* obj, const lua_Number n) {
    obj->value.n = n;
    obj->tt = LUA_TNUMBER;
}

inline void setpvalue(TValue* obj, void* p) {
    obj->value.p = p;
    obj->tt = LUA_TLIGHTUSERDATA;
}

inline void setbvalue(TValue* obj, const bool b) {
    obj->value.b = b;
    obj->tt = LUA_TBOOLEAN;
}

inline void setsvalue(TValue* obj, const TString* s) {
    obj->value.gc = (GCObject*)s;
    obj->tt = LUA_TSTRING;
}

inline void sethvalue(TValue* obj, const Table* h) {
    obj->value.gc = (GCObject*)h;
    obj->tt = LUA_TTABLE;
}

inline void setclvalue(TValue* obj, const Closure* cl) {
    obj->value.gc = (GCObject*)cl;
    obj->tt = LUA_TFUNCTION;
}

inline void setptvalue(TValue* obj, const Proto* pt) {
    obj->value.gc = (GCObject*)pt;
    obj->tt = LUA_TPROTO;
}

inline void setobj(TValue* desc, const TValue* src) {
    desc->value.gc = src->value.gc;
    desc->tt = src->tt;
}

const TValue luaO_nilobject_;
#define luaO_nilobject (&luaO_nilobject_)