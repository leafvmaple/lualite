#include "tparser.h"
#include "tlex.h"
#include "tstring.h"
#include "tparser.h"
#include "tfunc.h"
#include "ttable.h"

static void open_func(LexState* ls, FuncState* fs) {
    lua_State* L = ls->L;
    fs->f = luaF_newproto(L);
    fs->h = luaH_new(L, 0, 0);
    fs->prev = ls->fs;
}

Proto* luaY_parser(lua_State* L, ZIO* z, const char* name) {
    LexState ls;
    FuncState fs;

    luaX_setinput(L, &ls, z, luaS_new(L, name));
    open_func(&ls, &fs);
    sethvalue(L->top, fs.h);
    L->top++;
    setptvalue(L->top, fs.f);
    L->top++;

    return fs.f;
}