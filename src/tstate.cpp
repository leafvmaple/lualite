#include "tstate.h"
#include "ttable.h"
#include "tstring.h"
#include "tlex.h"
#include "tgc.h"

struct LG {
    lua_State l;
    global_State g;
};

void preinit_state(lua_State* L, global_State* g) {
    _G(L) = g;
    L->nCcalls = 0;
    setnilvalue(_gt(L));
}

void stack_init(lua_State* L) {
    L->base_ci.resize(BASIC_CI_SIZE);
    L->ci = &L->base_ci.front();

    L->stack.resize(BASIC_STACK_SIZE + EXTRA_STACK);

    L->ci->func = &L->stack.front();
    setnilvalue(L->ci->func);
    L->ci->base = L->ci->func + 1;
    L->ci->top = L->ci->base + LUA_MINSTACK;

    L->base = L->ci->base;
    L->top = L->base;
}

void f_luaopen(lua_State* L) {
    stack_init(L);

    sethvalue(_gt(L), luaH_new(L, 0, 2));
    sethvalue(_registry(L), luaH_new(L, 0, 2));

    luaS_resize(L, MINSTRTABSIZE);

    luaT_init(L);
    luaX_init(L);

    //g->GCthreshold = 4 * g->totalbytes;
}

lua_State* lua_newstate() {
    lua_State* L;
    global_State* g;

    // TODO: use LUAI_EXTRASPACE
    LG* l = new LG;

    L = &l->l;
    g = &l->g;

    g->currentwhite = bit2mask(WHITE0BIT, FIXEDBIT);
    L->marked = _luaC_white(g);
    L->tt = LUA_TTHREAD;

    preinit_state(L, g);
    g->strt.nuse = 0;

    f_luaopen(L);

    return L;
}