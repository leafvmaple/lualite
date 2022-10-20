#pragma once
#pragma warning(disable : 4200)

#include <list>
#include <vector>
#include <unordered_map>

#include "lua.h"
#include "tlimits.h"

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
		lu_byte reserved;      // �ַ���Ϊϵͳ������ʶ��ʱ�����ﲻΪ0
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
    TValue* k;  /* constants used by the function */
    Instruction* code;
    struct Proto** p;  /* functions defined inside the function */
};

// C�����е�ָ������ݶ��ڴ�������ݶ��У�ֻ��Ҫһ������ָ����ڼ���
struct CClosure : ClosureHeader {
    lua_CFunction f;    // ����ָ��
    TValue upvalue[0];
};

// Lua������ָ�������Ҫ���й��������Ҫһ��Proto���洢
struct LClosure : ClosureHeader {
    Proto* p;    // ����ԭ��
    // UpVal* upvals[1];
};


union Closure {
    CClosure c;
    LClosure l;
};

inline void setnilvalue(TValue* obj) {
    obj->tt = LUA_TNIL;
}

inline void setpvalue(TValue* obj, void* x) {
    obj->value.p = x;
    obj->tt = LUA_TLIGHTUSERDATA;
}

inline void setsvalue(TValue* obj, TString* x) {
    obj->value.gc = (GCObject*)x;
    obj->tt = LUA_TSTRING;
}

inline void sethvalue(TValue* obj, Table* x) {
    obj->value.gc = (GCObject*)x;
    obj->tt = LUA_TTABLE;
}

inline void setclvalue(TValue* obj, Closure* x) {
    obj->value.gc = (GCObject*)x;
    obj->tt = LUA_TFUNCTION;
}

inline void setobj(TValue* desc, const TValue* src) {
    desc->value.gc = src->value.gc;
    desc->tt = src->tt;
}

const TValue luaO_nilobject_;
#define luaO_nilobject (&luaO_nilobject_)