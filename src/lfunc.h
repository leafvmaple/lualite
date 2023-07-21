#pragma once

#include "lobject.h"

Proto* luaF_newproto(lua_State* L);
// ´´½¨CClosure
Closure* luaF_newCclosure(lua_State* L, int nelems, Table* e);
Closure* luaF_newLclosure(lua_State* L, int nelems, Table* e);