#pragma once

#include "lobject.h"
#include "lstate.h"

#define bitmask(b)	(1<<(b))
#define bit2mask(b1,b2)	(bitmask(b1) | bitmask(b2))

#define WHITE0BIT	0
#define WHITE1BIT	1
#define BLACKBIT	2
#define FINALIZEDBIT	3
#define KEYWEAKBIT	3
#define VALUEWEAKBIT	4
#define FIXEDBIT	5          // ²»»áGC
#define SFIXEDBIT	6
#define WHITEBITS	bit2mask(WHITE0BIT, WHITE1BIT)

#define setbits(x,m)	((x) |= (m))

#define l_setbit(x,b)	setbits(x, bitmask(b))

void luaC_link(lua_State* L, GCheader* o, TVALUE_TYPE tt);

inline lu_byte _luaC_white(global_State* g) {
    return static_cast<lu_byte>(g->currentwhite & WHITEBITS);
}