#pragma once

#include "tobject.h"
#include "tzio.h"

struct LexState;

enum expkind {
    VVOID,	/* no value */
    VNIL,
    VTRUE,
    VFALSE,
    VK,		/* info = index of constant in `k' */
    VKNUM,	/* nval = numerical value */
    VLOCAL,	/* info = local register */
    VUPVAL,       /* info = index of upvalue in `upvalues' */
    VGLOBAL,	/* info = index of table; aux = index of global name in `k' */
    VINDEXED,	/* info = table register; aux = index register (or `k') */
    VJMP,		/* info = instruction pc */
    VRELOCABLE,	/* info = instruction pc */
    VNONRELOC,	/* info = result register */
    VCALL,	/* info = instruction pc */
    VVARARG	/* info = instruction pc */
};

struct FuncState {
	Proto* f;
	Table* h;
	FuncState* prev;
    lua_State* L;
	LexState* ls;
    int nk = 0;
};

struct expdesc {
    expkind k;
    union {
        struct { int info, aux; } s;
    } u;
};

Proto* luaY_parser(lua_State* L, ZIO* z, const char* name);