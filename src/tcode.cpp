#include "tcode.h"
#include "tparser.h"
#include "tobject.h"
#include "ttable.h"
#include "tfunc.h"
#include "tlex.h"

// ��FuncState��¼һ����������
static int addk(FuncState* fs, TValue* k, TValue* v) {
    TValue* idx = luaH_set(fs->L, fs->h, k);
    
    int nk = (int)fs->f->k->size();
    setnvalue(idx, nk);
    fs->f->k->push_back(*v);
    return nk;
}

static int luaK_code(FuncState* fs, Instruction i, int line) {
    Proto* f = fs->f;
    f->code.push_back(i);
    f->lineinfo.push_back(line);
    return fs->pc++;
}

static void discharge2reg(FuncState* fs, expdesc* e, int reg) {
    luaK_dischargevars(fs, e);
    switch (e->k) {
    case VK: {
        luaK_codeABx(fs, OP_LOADK, reg, e->u.s.info);
        break;
    }
    case VRELOCABLE: {
        // �����·��䣬ֱ�Ӱ��¼Ĵ���λ���赽ָ��A�������ɣ�ָ���ͨ��A������ָ���Ĵ���ȡֵ��
        Instruction* pc = getcode(fs, e);
        SETARG_A(*pc, reg);
        break;
    }
    case VNONRELOC: {
        if (reg != e->u.s.info)
            luaK_codeABC(fs, OP_MOVE, reg, e->u.s.info, 0);
        break;
    }
    default: {
        return;  /* nothing to do... */
    }
    }
    e->u.s.info = reg;
    e->k = VNONRELOC;
}


static void exp2reg(FuncState* fs, expdesc* e, int reg) {
    discharge2reg(fs, e, reg);
    e->u.s.info = reg;
    e->k = VNONRELOC;
}


int luaK_codeABx(FuncState* fs, OpCode o, int A, unsigned int Bx) {
    return luaK_code(fs, CREATE_ABx(o, A, Bx), fs->ls->lastline);
}

int luaK_codeABC(FuncState* fs, OpCode o, int A, int B, int C) {
    return luaK_code(fs, CREATE_ABC(o, A, B, C), fs->ls->lastline);
}

void luaK_reserveregs(FuncState* fs, int n) {
    fs->freereg += n;
}

void luaK_dischargevars(FuncState* fs, expdesc* e) {
    switch (e->k) {
    case VGLOBAL: {
        // e->u.s.info��֮ǰ����ֵΪ��ʶ���������
        // code = fs->h[e->u.s.info]
        // A������ʱ��0���ȴ��ط����ʱ���ٸ�����ȷ��ֵ
        // �������e->u.s.infoָ��ָ��λ��
        e->u.s.info = luaK_codeABx(fs, OP_GETGLOBAL, 0, e->u.s.info);
        e->k = VRELOCABLE;
        break;
    }
    }
}

void luaK_exp2nextreg(FuncState* fs, expdesc* e) {
    luaK_dischargevars(fs, e);
    luaK_reserveregs(fs, 1);
    exp2reg(fs, e, fs->freereg - 1);
}

int luaK_stringK(FuncState* fs, TString* s) {
    TValue o;
    setsvalue(&o, s);
    return addk(fs, &o, &o);
}
