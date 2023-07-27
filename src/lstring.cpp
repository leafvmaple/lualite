#include "lstring.h"
#include "lstate.h"
#include "lgc.h"

void luaS_resize(lua_State* L, int newsize) {
    _G(L)->strt.hash.reserve(newsize);;
}

TString* newlstr(lua_State* L, const char* str, size_t l) {
    TString* ts = nullptr;
    stringtable* tb = nullptr;

    ts = new TString;
    ts->len = l;
    ts->hash = 0;
    ts->marked = _luaC_white(_G(L));
    ts->tt = LUA_TSTRING;
    ts->reserved = 0;
    ts->s = std::string(str, l);

    tb = &_G(L)->strt;
    tb->hash[ts->s] = ts;
    tb->nuse++;

    return ts;
}

TString* luaS_newlstr(lua_State* L, const char* str, size_t l) {
    GCheader* o = _G(L)->strt.hash[str];
    if (o) {
        return static_cast<TString*>(o);
    }
    return newlstr(L, str, l);
}