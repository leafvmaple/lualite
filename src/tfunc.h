#pragma once

#include "tobject.h"

Proto* luaF_newproto(lua_State* L);
Closure* luaF_newCclosure(lua_State* L, int nelems, Table* e);