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
    fs->L = ls->L;
    ls->fs = fs;

    sethvalue(L->top++, fs->h, "#[open_func] Const String#");
    setptvalue(L->top++, fs->f, "#[open_func] Proto#");
}

static void chunk(LexState* ls);

// 将文件解析为一个Proto
Proto* luaY_parser(lua_State* L, ZIO* z, const char* name) {
    LexState ls;
    FuncState fs;

    luaX_setinput(L, &ls, z, luaS_new(L, name));
    open_func(&ls, &fs);
    luaX_next(&ls);
    chunk(&ls);

    return fs.f;
}

// 是否为block结束
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

static BinOpr getbinopr(int op) {
    switch (op) {
    case '+': return OPR_ADD;
    case '-': return OPR_SUB;
    case '*': return OPR_MUL;
    case '/': return OPR_DIV;
    case '%': return OPR_MOD;
    case '^': return OPR_POW;
    case TK_CONCAT: return OPR_CONCAT;
    case TK_NE: return OPR_NE;
    case TK_EQ: return OPR_EQ;
    case '<': return OPR_LT;
    case TK_LE: return OPR_LE;
    case '>': return OPR_GT;
    case TK_GE: return OPR_GE;
    case TK_AND: return OPR_AND;
    case TK_OR: return OPR_OR;
    default: return OPR_NOBINOPR;
    }
}

/*
** subexpr -> (simpleexp | unop subexpr) { binop subexpr }
** where `binop' is any binary operator with a priority higher than `limit'
*/
static BinOpr subexpr(LexState* ls, expdesc* v, unsigned int limit) {
    BinOpr op = OPR_NOBINOPR;

    ls->L->nCcalls++;
    simpleexp(ls, v);
    op = getbinopr(ls->t.token);
    ls->L->nCcalls--;

    return op;
}

static void expr(LexState* ls, expdesc* v) {
    subexpr(ls, v, 0);
}

// 获取Token是否等于c，是则获取下一个Token并返回true
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

    return 0;
}

static void check_match(LexState* ls, int what) {
    testnext(ls, what);
}

static void funcargs(LexState* ls, expdesc* f) {
    int base = 0;
    int nparams = 0;
    FuncState* fs = ls->fs;
    expdesc args;

    switch (ls->t.token) { 
    case '(': {
        luaX_next(ls);
        if (ls->t.token == ')')  /* arg list is empty? */
            args.k = VVOID;
        else
            explist1(ls, &args);
        check_match(ls, ')');
        break;
    }
    default: {
        return;
    }
    }
    base = f->u.s.info;
    if (args.k != VVOID)
        luaK_exp2nextreg(fs, &args);
    nparams = fs->freereg - (base + 1);
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

/* primaryexp -> prefixexp { `.' NAME | `[' exp `]' | `:' NAME funcargs | funcargs } */
static void primaryexp(LexState* ls, expdesc* v) {
    FuncState* fs = ls->fs;
    prefixexp(ls, v);
    while (true) {
        switch (ls->t.token)
        {
        case '(': {
            luaK_exp2nextreg(fs, v);
            funcargs(ls, v);
            break;
        }
        default: return;
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