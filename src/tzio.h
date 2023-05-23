#pragma once

#include "lua.h"

#define EOZ	(-1)			/* end of stream */

struct ZIO {
    size_t n;
    const char* p;
    lua_Reader reader;
    void* data;
    lua_State* L;
};

int zgetc(ZIO* z);

void luaZ_init(lua_State* L, ZIO* z, lua_Reader reader, void* data);
int luaZ_fill(ZIO* z);
