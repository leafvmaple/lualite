#include "tparser.h"
#include "tlex.h"
#include "tstring.h"
#include "tparser.h"
#include "tfunc.h"
#include "ttable.h"
#include "tcode.h"

static void open_func(LexState* ls, FuncState* fs) {
    lua_State* L = ls->L;
    fs->f = luaF_newproto(L);
    fs->h = luaH_new(L, 0, 0);
    fs->prev = ls->fs;
    fs->ls = ls;
    fs->pc = 0;
    fs->nk = 0;
    ls->fs = fs;

    sethvalue(L->top++, fs->h);
    setptvalue(L->top++, fs->f);
}

static void chunk(LexState* ls);

Proto* luaY_parser(lua_State* L, ZIO* z, const char* name) {
    LexState ls;
    FuncState fs;

    luaX_setinput(L, &ls, z, luaS_new(L, name));
    open_func(&ls, &fs);
    luaX_next(&ls);
    chunk(&ls);

    return fs.f;
}

static bool block_follow(int token) {
    switch (token)
    {
    case TK_EOS:
        return true;
    default:
        return false;
    }
}

static void init_exp(expdesc* e, expkind k, int i) {
    e->k = k;
    e->u.s.info = i;
}

static void codestring(LexState* ls, expdesc* e, TString* s) {
	init_exp(e, VK, luaK_stringK(ls->fs, s));
}

static void simpleexp(LexState* ls, expdesc* v) {
    switch (ls->t.token)
    {
    case TK_STRING: {
        codestring(ls, v, ls->t.seminfo.ts);
        break;
    }
    default:
        break;
    }
    luaX_next(ls);
}

static BinOpr subexpr(LexState* ls, expdesc* v, unsigned int limit) {
    BinOpr op;

    ls->L->nCcalls++;
    simpleexp(ls, v);
    ls->L->nCcalls--;
}

static void expr(LexState* ls, expdesc* v) {
	subexpr(ls, v, 0);
}

static int testnext(LexState* ls, int c) {
	if (ls->t.token == c) {
		luaX_next(ls);
		return 1;
	}
	return 0;
}

static int explist1(LexState* ls, expdesc* v) {
    int n = 1;
    expr(ls, v);
    while (testnext(ls, ',')) {
        n++;
    }
}

static void funcargs(LexState* ls, expdesc* f) {
    int base = 0;
    int nparams = 0;
    expdesc args;

    switch (ls->t.token) {
    case '(': {
        luaX_next(ls);
		if (ls->t.token == ')')  /* arg list is empty? */
			args.k = VVOID;
		else
			explist1(ls, &args);
        break;
    }
    default: {
        break;
    }
    }
    base = f->u.s.info;
    init_exp(f, VCALL, luaK_codeABC(ls->fs, OP_CALL, base, nparams + 1, 2));
}

static int singlevaraux(FuncState* fs, TString* n, expdesc* var, int base) {
    if (fs == nullptr) {
        init_exp(var, VGLOBAL, NO_REG);
        return VGLOBAL;
    }
    if (singlevaraux(fs->prev, n, var, 0) == VGLOBAL)
        return VGLOBAL;
}

static TString* str_checkname(LexState* ls) {
    TString* ts = ls->t.seminfo.ts;
    luaX_next(ls);
    return ts;
}

static void singlevar(LexState* ls, expdesc* var) {
    TString* varname = str_checkname(ls);
    if (singlevaraux(ls->fs, varname, var, 1) == VGLOBAL)
        var->u.s.info = luaK_stringK(ls->fs, varname);
}

static void prefixexp(LexState* ls, expdesc* var) {
    switch (ls->t.token)
    {
    case TK_NAME: {
        singlevar(ls, var);
        break;
    }
    default:
        break;
    }
}

static void primaryexp(LexState* ls, expdesc* v) {
    prefixexp(ls, v);
    while (true) {
        switch (ls->t.token)
        {
        case '(': {
            funcargs(ls, v);
            break;
        }
        default:
            break;
        }
    }
}

static void exprstat(LexState* ls) {
    FuncState* fs = ls->fs;
    expdesc desc;
    primaryexp(ls, &desc);
}

static void statement(LexState* ls) {
    switch (ls->t.token)
    {
    default: {
        exprstat(ls);
    }
    }
}

static void chunk(LexState* ls) {
    ++ls->L->nCcalls;
    while (!block_follow(ls->t.token)) {
        statement(ls);
    }
    --ls->L->nCcalls;
}