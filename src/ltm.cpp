#include "ltm.h"
#include "lstring.h"
#include "lstate.h"

// ³õÊ¼»¯metatable¹Ø¼ü×Ö
void luaT_init(lua_State* L) {
    static const char* const luaT_eventname[] = {  /* ORDER TM */
      "__index",
      "__newindex",
      "__gc",
      "__mode",
      "__eq",
      "__add",
      "__sub",
      "__mul",
      "__div",
      "__mod",
      "__pow",
      "__unm",
      "__len",
      "__lt",
      "__le",
      "__concat",
      "__call"
    };

    for (int i = 0; i < TM_N; i++) {
        _G(L)->tmname[i] = luaS_new(L, luaT_eventname[i]);
        _luaS_fix(_G(L)->tmname[i]);  /* never collect these names */
    }
}