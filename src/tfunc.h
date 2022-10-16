#pragma once

#include "tobject.h"

Closure* luaF_newCclosure(lua_State* L, int nelems, Table* e);