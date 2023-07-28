#pragma once
#pragma warning(disable : 4200)

#include <list>
#include <vector>
#include <unordered_map>
#include <bitset>

#include "lua.h"
#include "llimits.h"

typedef double lua_Number;

struct TValue;
union GCObject;

#define FIRST_RESERVED	257

enum RESERVED {
    /* terminal symbols denoted by reserved words */
    TK_NONE,
    TK_AND = FIRST_RESERVED,
    TK_BREAK,
    TK_DO,
    TK_ELSE,
    TK_ELSEIF,
    TK_END,
    TK_FALSE,
    TK_FOR,
    TK_FUNCTION,
    TK_IF,
    TK_IN,
    TK_LOCAL,
    TK_NIL,
    TK_NOT,
    TK_OR,
    TK_REPEAT,
    TK_RETURN,
    TK_THEN,
    TK_TRUE,
    TK_UNTIL,
    TK_WHILE,
    TK_RESERVED_COUNT,
    /* other terminal symbols */
    TK_CONCAT = TK_RESERVED_COUNT,
    TK_DOTS,
    TK_EQ,
    TK_GE,
    TK_LE,
    TK_NE,
    TK_NUMBER,
    TK_NAME,
    TK_STRING,
    TK_EOS
};

#define NUM_RESERVED	(TK_RESERVED_COUNT - FIRST_RESERVED)

struct GCheader{
    // GCObject* next;
    TVALUE_TYPE tt = LUA_TNIL;
    std::bitset<8> marked;

    virtual ~GCheader() {}
};

union Value {
    GCheader* gc = nullptr;  // ���������ͣ�ֻ��ָ�룬��ҪGC
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

struct TString: GCheader {
    RESERVED reserved = TK_NONE;      // �ַ���Ϊϵͳ������ʶ��ʱ�����ﲻΪ0
    unsigned int hash = 0;
    std::string s;
};

inline size_t keyhash(const TValue& k) {
    switch (k.tt) {
    case LUA_TNUMBER: {
        return (size_t)k.value.n;
    }
    case LUA_TSTRING: {
        TString* s = (TString*)k.value.gc;
        return std::hash<std::string>{}(s->s);
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
    Table* metatable = nullptr;
    std::vector<TValue> array;
    std::unordered_map<const TValue, TValue, KeyFunction> node;
};


struct Closure : GCheader {
    lu_byte isC       = false;
    GCObject* gclist  = nullptr;
    Table* env        = nullptr;
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
};

// �������������ݵ�������ִ��ģ��
struct Proto : GCheader {
    std::vector<TValue> k; /* ���ú������õ��ĳ��� */
    std::vector<Instruction> code; /* ָ���б� */
    std::vector<int> lineinfo; /* ָ���б���ÿ��ָ�����ڵ�line */
    Proto** p = nullptr;  /* ������Ƕ�׺��� */
};

// C�����е�ָ������ݶ��ڴ�������ݶ��У�ֻ��Ҫһ������ָ����ڼ���
struct CClosure : Closure {
    lua_CFunction f = nullptr;    // ����ָ��
    std::vector<TValue> upvalue;
};

// Lua������ָ���������Ҫ���й��������Ҫһ��Proto���洢
struct LClosure : Closure {
    Proto* p = nullptr;    // ����ԭ��
    std::vector<UpVal*> upvals;
};

inline bool ttisnumber(TValue* obj) {
    return obj->tt == LUA_TNUMBER;
}
inline bool ttisstring(TValue* obj) {
    return obj->tt == LUA_TSTRING;
}

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

int luaO_str2d(const char* s, lua_Number* result);