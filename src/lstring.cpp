#include "lstring.h"
#include "lstate.h"
#include "lgc.h"

void luaS_resize(lua_State* L, int newsize) {
    _G(L)->strt.hash.reserve(newsize);;
}

TString* newlstr(lua_State* L, const char* str, size_t l) {
    TString* ts = nullptr;
    stringtable* tb = nullptr;

    ts = (TString*)malloc(sizeof(TString) + (l + 1));
    ts->tsv.len = l;
    ts->tsv.hash = 0;
    ts->tsv.marked = _luaC_white(_G(L));
    ts->tsv.tt = LUA_TSTRING;
    ts->tsv.reserved = 0;

    memcpy(ts->tsv.s, str, l * sizeof(char));
    ts->tsv.s[l] = '\0';

    tb = &_G(L)->strt;
    tb->hash[str] = &ts->tsv;
    tb->nuse++;

    return ts;
}

TString* luaS_newlstr(lua_State* L, const char* str, size_t l) {
    GCheader* o = _G(L)->strt.hash[str];
    if (o) {
        return (TString*)o;
    }
    return newlstr(L, str, l);
}