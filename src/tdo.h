#pragma once

#include "tobject.h"
#include "tstate.h"
#include "tzio.h"

#define PCRLUA		0	/* initiated a call to a Lua function */
#define PCRC		1	/* did a call to a C function */

struct CCallS {  /* data to `f_Ccall' */
    lua_CFunction func;
    void* ud;
};

inline ptrdiff_t savestack(lua_State* L, TValue* p) {
    return (char*)(p) - (char*)&L->stack.front();
}

int luaD_precall(lua_State* L, TValue* func, int nresults);
void luaD_call(lua_State* L, TValue* func, int nResults);

int luaD_protectedparser(lua_State* L, ZIO* z, const char* name);
