#pragma once

#include "tobject.h"
#include "tstate.h"

#define PCRLUA		0	/* initiated a call to a Lua function */
#define PCRC		1	/* did a call to a C function */

struct CCallS {  /* data to `f_Ccall' */
    lua_CFunction func;
    void* ud;
};

inline ptrdiff_t _savestack(lua_State* L, TValue* p) {
    return (char*)(p) - (char*)&L->stack.front();
}

int luaD_precall(lua_State* L, TValue* func, int nresults);
void luaD_call(lua_State* L, TValue* func, int nResults);

template<typename T>
int luaD_pcall(lua_State* L, void (*func)(lua_State* L, T* ud), T* u, ptrdiff_t oldtop);

// explicit
template int luaD_pcall(lua_State* L, void (*func)(lua_State* L, CCallS* ud), CCallS* u, ptrdiff_t oldtop);