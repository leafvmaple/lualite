#include "lgc.h"
#include "lstate.h"

// ����GC����ҵ�GC�б��У�ͬʱ���Ϊ��ɫ
void luaC_link(lua_State* L, GCheader* o, TVALUE_TYPE tt) {
    global_State* g = _G(L);

    g->rootgc.push_back(o);
    o->tt = tt;
    o->marked = _luaC_white(g);
}