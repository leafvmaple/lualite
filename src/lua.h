#pragma once

struct lua_State;

#define LUA_VERSION     "Lua Lite 0.1.0"
#define LUA_COPYRIGHT   "Copyright (C) 2022-2023 lvmaple.com, Zohar Lee"

#define LUA_TNONE		(-1)

enum TVALUE_TYPE {
    LUA_TNIL,
    LUA_TBOOLEAN,
    LUA_TLIGHTUSERDATA,
    LUA_TNUMBER,
    LUA_TSTRING,
    LUA_TTABLE,
    LUA_TFUNCTION,
    LUA_TUSERDATA,
    LUA_TTHREAD,
    LAST_TAG = LUA_TTHREAD,

    LUA_TPROTO,
    NUM_TAGS = LUA_TPROTO,
    LUA_TUPVAL,
    LUA_TDEADKEY,
};



#define LUA_REGISTRYINDEX	(-10000)
#define LUA_ENVIRONINDEX	(-10001)
#define LUA_GLOBALSINDEX	(-10002)

#define _DECL , char const* debug = nullptr
#define _IMPL , char const* debug

typedef int (*lua_CFunction) (lua_State* L);
typedef const char* (*lua_Reader) (lua_State* L, void* ud, size_t* sz);

lua_State* lua_newstate();

void lua_pushnil(lua_State* L);
void lua_pushlstring(lua_State* L, const char* s, size_t l _DECL);
void lua_pushstring(lua_State* L, const char* s _DECL);
void lua_pushvalue(lua_State* L, int idx);
void lua_pushcclosure(lua_State* L, lua_CFunction fn, int n _DECL);
void lua_pushcfunction(lua_State* L, lua_CFunction f _DECL);

void* lua_touserdata(lua_State* L, int idx);

void lua_gettable(lua_State* L, int idx);
void lua_getfield(lua_State* L, int idx, const char* k);
inline void lua_getglobal(lua_State* L, int idx, const char* k);

void lua_settable(lua_State* L, int idx);
void lua_setfield(lua_State* L, int idx, const char* k);
inline void lua_setglobal(lua_State* L, const char* s) {
    lua_setfield(L, LUA_GLOBALSINDEX, s);
}

void lua_createtable(lua_State* L, int narr, int nrec _DECL);

int lua_gettop(lua_State* L);
void lua_settop(lua_State* L, int idx);
void lua_pop(lua_State* L, int n);
void lua_remove(lua_State* L, int idx);

bool lua_isnil(lua_State* L, int idx);
bool lua_istable(lua_State* L, int idx);
int  lua_type(lua_State* L, int idx);

void lua_call(lua_State* L, int nargs, int nresults);        // ����һ��Lua����
int  lua_cpcall(lua_State* L, lua_CFunction func, void* ud); // ����һ��C����
int  lua_load(lua_State* L, lua_Reader reader, void* dt, const char* chunkname); // ���ش���

const char* lua_tolstring(lua_State* L, int idx, size_t* len);
const char* lua_tostring(lua_State* L, int idx);