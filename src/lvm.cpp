#include "lvm.h"
#include "ldo.h"
#include "ltable.h"
#include "lstate.h"
#include "lopcodes.h"

inline TValue* RA(TValue* base, Instruction i) {
    return base + GETARG_A(i);
}
inline TValue* RB(TValue* base, Instruction i) {
    return base + GETARG_B(i);
}
inline TValue* RC(TValue* base, Instruction i) {
    return base + GETARG_C(i);
}
inline TValue* KBx(TValue* base, Instruction i) {
    return base + GETARG_C(i);
}

void luaV_gettable(lua_State* L, const TValue* t, TValue* key, TValue* val) {
    Table* h = (Table*)t->value.gc;
    const TValue* res = luaH_get(h, key);
    setobj(val, res);
}

void luaV_settable(lua_State* L, const TValue* t, TValue* key, TValue* val) {
    TValue* oldval = luaH_set(L, (Table*)t->value.gc, key);
    setobj(oldval, val);
}

void luaV_execute(lua_State* L, int nexeccalls) {
    const Instruction* pc = nullptr;
    LClosure* cl = nullptr;
    TValue* base = nullptr;
    TValue* k = nullptr;

    pc = L->savedpc;
    cl = &L->ci->func->value.gc->cl.l;
    base = L->base;
    k = &cl->p->k->front();

    while (true) {
        const Instruction i = *pc++;
        TValue* ra = RA(base, i);

        switch (GET_OPCODE(i))
        {
        // Load Const
        case OP_LOADK: {
            setobj(ra, KBx(k, i));
            continue;
        }
        case OP_GETGLOBAL: {
            TValue g;
            TValue* rb = KBx(k, i);
            sethvalue(&g, cl->env);
            luaV_gettable(L, &g, rb, ra);
            continue;
        }
        case OP_RETURN: {
            int b = GETARG_B(i);
            if (b != 0) L->top = ra + b - 1;
            L->savedpc = pc;
            // b = luaD_poscall(L, ra);
            if (--nexeccalls == 0)  /* was previous function running `here'? */
                return;  /* no: return */
        }
        case OP_CALL: {
            // bΪ������+��������
            int b = GETARG_B(i);
            if (b != 0) L->top = ra + b;
            L->savedpc = pc;
            luaD_precall(L, ra, 0);
            continue;
        }
        case OP_SETGLOBAL: {
            TValue g;
            sethvalue(&g, cl->env);
            luaV_settable(L, &g, KBx(k, i), ra);
            continue;
        }
        default:
            break;
        }
    }
}
