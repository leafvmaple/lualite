#include "tcode.h"
#include "tparser.h"
#include "tobject.h"
#include "ttable.h"
#include "tfunc.h"
#include "tlex.h"

static int addk(FuncState* fs, TValue* k, TValue* v) {
    TValue* idx = luaH_set(fs->L, fs->h, k);
    
    int nk = (int)fs->f->k->size();
    setnvalue(idx, nk);
    fs->f->k->push_back(v);
    return nk;
}

static int luaK_code(FuncState* fs, Instruction i, int line) {
    Proto* f = fs->f;
    f->code->push_back(i);
    f->lineinfo->push_back(line);
    return fs->pc++;
}

int luaK_codeABC(FuncState* fs, OpCode o, int A, int B, int C) {
    return luaK_code(fs, CREATE_ABC(o, A, B, C), fs->ls->lastline);
}

int luaK_stringK(FuncState* fs, TString* s) {
    TValue o;
    setsvalue(&o, s);
    return addk(fs, &o, &o);
}
