#include "tgc.h"
#include "tstate.h"

// 将可GC对象挂到GC列表中，同时标记为白色
void luaC_link(lua_State* L, GCheader* o, lu_byte tt) {
    global_State* g = _G(L);

    g->rootgc.push_back(o);
    o->tt = tt;
    o->marked = _luaC_white(g);
}