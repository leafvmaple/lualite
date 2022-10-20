#pragma once

#include "tobject.h"
#include "tzio.h"

struct FuncState {
	Proto* f;
	Table* h;
	FuncState* prev;
};

Proto* luaY_parser(lua_State* L, ZIO* z, const char* name);