#include "ttable.h"

#include "tobject.h"
#include "tgc.h"

// Table Ïà¹Ø

void setarrayvector(lua_State* L, Table* t, int size) {
    t->array = new std::vector<TValue>;
    t->array->resize(size);
}

void setnodevector(lua_State* L, Table* t, int size) {
    // t->node = new std::unordered_map<const TValue, TValue, KeyFunction>;
    t->node.reserve(size);
}

Table* luaH_new(lua_State* L, int narray, int nhash) {
    Table* t = new Table;
    luaC_link(L, t, LUA_TTABLE);

    setarrayvector(L, t, narray);
    setnodevector(L, t, nhash);

    return t;
}

TValue* newkey(lua_State* L, Table* t, const TValue* key) {
    return &t->node[*key];
}

const TValue* luaH_getstr(Table* t, const TString* key) {
    TValue o;
    setsvalue(&o, key);
    auto it = t->node.find(o);
    if (it != t->node.end())
        return &(*it).second;
    return luaO_nilobject;
}

const TValue* luaH_get(Table* t, const TValue* key) {
    switch (key->tt) {
    case LUA_TNIL: return luaO_nilobject;
    case LUA_TSTRING: return luaH_getstr(t, (TString*)key->value.gc);
    }
}


TValue* luaH_setstr(lua_State* L, Table* t, const TString* key){
    const TValue* p = luaH_getstr(t, key);
    if (p != luaO_nilobject)
        return (TValue*)p;
    TValue k;
    setsvalue(&k, key);
    return newkey(L, t, &k);
}

TValue* luaH_set(lua_State* L, Table* t, const TValue* key) {
    const TValue* p = luaH_get(t, key);
    if (p != luaO_nilobject)
        return (TValue*) p;
    return newkey(L, t, key);
}
