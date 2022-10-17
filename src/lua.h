#pragma once

struct lua_State;

#define LUA_VERSION	"Lua Lite 0.1"

#define LUA_TNONE		(-1)

#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8

#define LUA_REGISTRYINDEX	(-10000)
#define LUA_ENVIRONINDEX	(-10001)
#define LUA_GLOBALSINDEX	(-10002)

typedef int (*lua_CFunction) (lua_State* L);

void lua_pushnil(lua_State* L);
void lua_pushlstring(lua_State* L, const char* s, size_t l);
void lua_pushstring(lua_State* L, const char* s);
void lua_pushvalue(lua_State* L, int idx);
void lua_pushcclosure(lua_State* L, lua_CFunction fn, int n);
inline void lua_pushcfunction(lua_State* L, lua_CFunction f) {
	lua_pushcclosure(L, f, 0);
}

void* lua_touserdata(lua_State* L, int idx);

void lua_gettable(lua_State* L, int idx);
void lua_getfield(lua_State* L, int idx, const char* k);

void lua_setfield(lua_State* L, int idx, const char* k);

inline void lua_setglobal(lua_State* L, const char* s) {
	lua_setfield(L, LUA_GLOBALSINDEX, s);
}

void lua_settop(lua_State* L, int idx);
void lua_pop(lua_State* L, int n);

bool lua_isnil(lua_State* L, int idx);
bool lua_istable(lua_State* L, int idx);
int  lua_type(lua_State* L, int idx);

void lua_call(lua_State* L, int nargs, int nresults);
int  lua_cpcall(lua_State* L, lua_CFunction func, void* ud);