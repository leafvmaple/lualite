#pragma once
#pragma warning(disable : 4200)

#include <list>
#include <vector>
#include <unordered_map>

#include "lua.h"
#include "llimits.h"

typedef double lua_Number;

struct TValue;
union GCObject;

struct GCheader {
    // GCObject* next;
    lu_byte tt;
    lu_byte marked;

    ~GCheader() {}
};

union Value {
    GCObject* gc = nullptr;  // ���������ͣ�ֻ��ָ�룬��ҪGC
    void* p;
    lua_Number n;
    int b;
};

struct TValue {
#ifdef _DEBUG
    std::string name;
#endif
    Value value;
    TVALUE_TYPE tt = LUA_TNIL;

    ~TValue() {}

    bool operator==(const TValue& k) const;
};

#ifdef _DEBUG
#define SET_DEBUG_NAME(t, s) if (s) {(((t)->name) = (s));}
#define COPY_DEBUG_NAME(d, s) (((d)->name) = (s)->name)
#else
#define SET_DEBUG_NAME(t, s)
#define COPY_DEBUG_NAME(d, s)
#endif

union TString {
    L_Umaxalign dummy;  /* ensures maximum alignment for strings */
    struct Genuine : GCheader {
        lu_byte reserved;      // �ַ���Ϊϵͳ������ʶ��ʱ�����ﲻΪ0
        unsigned int hash;
        size_t len;
        char s[0];

        ~Genuine() {}
    } tsv;

    ~TString() {}
};

inline size_t keyhash(const TValue& k) {
    switch (k.tt) {
    case LUA_TSTRING: {
        TString* s = (TString*)k.value.gc;
        return std::hash<const char*>{}(s->tsv.s);
    }
    default: {
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
    std::unordered_map<const TValue, TValue, KeyFunction> node;
};


struct ClosureHeader : GCheader {
    lu_byte isC;
    lu_byte nupvalues;
    GCObject* gclist;
    Table* env;
};

struct UpVal : GCheader {
    TValue* v = nullptr;  /* points to stack or to its own value */
    union Practical {
        TValue value;  /* the value (when closed) */
        struct {  /* double linked list (when open) */
            struct UpVal* prev = 0;
            struct UpVal* next = 0;
        } l;

        ~Practical() {}
    } u;

    ~UpVal() {}
};

struct Proto : GCheader {
    std::vector<TValue>* k; /* ���ú������õ��ĳ��� */
    std::vector<Instruction> code; /* ָ���б� */
    std::vector<int> lineinfo; /* ָ���б���ÿ��ָ�����ڵ�line */
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
    UpVal* upvals[1];
};

union Closure {
    CClosure c;
    LClosure l;

    ~Closure() {}
};

bool ttisnumber(TValue* obj);

void setnilvalue(TValue* obj _DECL);
void setnvalue(TValue* obj, const lua_Number n _DECL);
void setpvalue(TValue* obj, void* p _DECL);
void setbvalue(TValue* obj, const bool b _DECL);
void setsvalue(TValue* obj, const TString* s _DECL);
void sethvalue(TValue* obj, const Table* h _DECL);
void setclvalue(TValue* obj, const Closure* cl _DECL);
void setptvalue(TValue* obj, const Proto* pt _DECL);
void setobj(TValue* desc, const TValue* src);

const TValue luaO_nilobject_;
#define luaO_nilobject (&luaO_nilobject_)