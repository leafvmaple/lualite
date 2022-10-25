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
    std::vector<TValue*>* k; /* ���ú������õ��ĳ��� */
    std::vector<Instruction>* code;
    std::vector<int>* lineinfo;
    struct Proto** p;  /* ������Ƕ�׺��� */
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

bool ttisnumber(TValue* obj);

void setnilvalue(TValue* obj);
void setnvalue(TValue* obj, const lua_Number n);
void setpvalue(TValue* obj, void* p);
void setbvalue(TValue* obj, const bool b);
void setsvalue(TValue* obj, const TString* s);
void sethvalue(TValue* obj, const Table* h);
void setclvalue(TValue* obj, const Closure* cl);
void setptvalue(TValue* obj, const Proto* pt);
void setobj(TValue* desc, const TValue* src);

const TValue luaO_nilobject_;
#define luaO_nilobject (&luaO_nilobject_)