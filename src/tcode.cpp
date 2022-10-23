#include "tcode.h"
#include "tparser.h"
#include "tobject.h"
#include "ttable.h"
#include "tfunc.h"

static int addk(FuncState* fs, TValue* k, TValue* v) {
    TValue* idx = luaH_set(fs->L, fs->h, k);
    
    int nk = (int)fs->f->k->size();
    setnvalue(idx, nk);
    fs->f->k->push_back(v);
    return nk;
}

int luaK_stringK(FuncState* fs, TString* s) {
    TValue o;
    setsvalue(&o, s);
    return addk(fs, &o, &o);
}
