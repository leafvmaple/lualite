#pragma once
#pragma warning(disable : 4200)

#include <list>
#include <vector>
#include <unordered_map>

#include "lua.h"
#include "tlimits.h"

typedef double lua_Number;

struct TValue;

struct GCObject {
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
	struct : GCObject {
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


struct Table : GCObject {
    Table* metatable;
    std::vector<TValue>* array;
    std::unordered_map<const TValue, TValue, KeyFunction>* node;
};


struct ClosureHeader : GCObject {
    lu_byte isC;
    lu_byte nupvalues;
    GCObject* gclist;
    Table* env;
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

inline void _setsvalue(TValue* obj, TString* x) {
    obj->value.gc = &x->tsv;
    obj->tt = LUA_TSTRING;
}

inline void _sethvalue(TValue* obj, Table* x) {
    obj->value.gc = x;
    obj->tt = LUA_TTABLE;
}

inline void _setclvalue(TValue* obj, Closure* x) {
    obj->value.gc = &x->c;
    obj->tt = LUA_TFUNCTION;
}

inline void _setobj(TValue* desc, const TValue* src) {
    desc->value = src->value;
    desc->tt = src->tt;
}

const TValue luaO_nilobject_;
#define luaO_nilobject (&luaO_nilobject_)