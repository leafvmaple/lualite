#pragma once

// ** Lua Lite virtual machine

#include "lua.h"
#include "tobject.h"

void luaV_gettable(lua_State* L, const TValue* t, TValue* key, TValue* val);

void luaV_settable(lua_State* L, const TValue* t, TValue* key, TValue* val);

void luaV_execute(lua_State* L, int nexeccalls);