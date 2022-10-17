#pragma once

#include "tobject.h"

Table* luaH_new(lua_State* L, int narray, int nhash);
const TValue* luaH_getstr(Table* t, TString* key);
const TValue* luaH_get(Table* t, const TValue* key);
TValue* luaH_set(lua_State* L, Table* t, const TValue* key);