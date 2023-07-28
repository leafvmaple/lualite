#include "lstate.h"
#include "ltable.h"
#include "lstring.h"
#include "llex.h"
#include "lgc.h"

struct LG {
    lua_State l;
    global_State g;
};

// ��ʼ��lua_State
// 1. ��ʼ��l_Gȫ��״̬��
// 2. ��ʼ��ȫ�ֱ�����
void preinit_state(lua_State* L, global_State* g) {
    G(L) = g;
    setnilvalue(_gt(L), "#Global Table#");
}

// 1. ��ʼ��base_ci
// 2. ��ʼ��stack
void stack_init(lua_State* L) {
    L->base_ci.resize(BASIC_CI_SIZE);
    L->ci = &L->base_ci.front();

    L->stack.resize(BASIC_STACK_SIZE + EXTRA_STACK);

    L->ci->func = &L->stack.front();
    setnilvalue(L->ci->func, "#[stack_init] Function Entry#");
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

    g->currentwhite.set(WHITE0BIT);
    g->currentwhite.set(FIXEDBIT);
    L->marked = luaC_white(g);
    L->tt = LUA_TTHREAD;

    preinit_state(L, g);
    g->strt.nuse = 0;
    g->rootgc.push_back(L);

    f_luaopen(L);

    return L;
}