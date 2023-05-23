#include <ctype.h>

#include "tlex.h"
#include "tstring.h"
#include "tparser.h"
#include "ttable.h"

void next(LexState* ls) {
    ls->current = zgetc(ls->z);
}

void save(LexState* ls, int c) {
    ls->buff.push_back(c);
}

void save_and_next(LexState* ls) {
    save(ls, ls->current);
    next(ls);
}

const char* const luaX_tokens[] = {
    "and",
    "break",
    "do",
    "else",
    "elseif",
    "end",
    "false",
    "for",
    "function",
    "if",
    "in",
    "local",
    "nil",
    "not",
    "or",
    "repeat",
    "return",
    "then",
    "true",
    "until",
    "while",
    "..", "...", "==", ">=", "<=", "~=",
    "<number>", "<name>", "<string>", "<eof>",
    NULL
};

void luaX_init(lua_State* L) {
    for (int i = 0; i < NUM_RESERVED; i++) {
        TString* ts = luaS_new(L, luaX_tokens[i]);
        ts->tsv.reserved = (lu_byte)(i + 1);
    }
}


TString* luaX_newstring(LexState* ls, const char* str, size_t l) {
    lua_State* L = ls->L;
    TString* ts = luaS_newlstr(L, str, l);
    TValue* o = luaH_setstr(L, ls->fs->h, ts);
    setbvalue(o, true);
    return ts;
}

void luaX_setinput(lua_State* L, LexState* ls, ZIO* z, TString* source) {
    ls->L = L;
    ls->z = z;
    ls->source = source;
    next(ls);
}

static int llex(LexState* ls, SemInfo* semInfo) {
    ls->buff.clear();
    while (true) {
        switch (ls->current) {
        case EOZ: {
            return TK_EOS;
        }
        default: {
            if (isspace(ls->current)) {
                next(ls);
                continue;
            }
            else if (isalpha(ls->current) || ls->current == '_') {
                TString* ts = nullptr;
                do {
                    save_and_next(ls);
                } while (isalnum(ls->current) || ls->current == '_');
                ts = luaX_newstring(ls, ls->buff.c_str(), ls->buff.size());
                if (ts->tsv.reserved)
                    return ts->tsv.reserved - 1 + FIRST_RESERVED;
                else {
                    semInfo->ts = ts;
                    return TK_NAME;
                }
            }
        }
        }
    }
}

void luaX_next(LexState* ls){
    ls->lastline = ls->linenumber;
    ls->t.token = llex(ls, &ls->t.seminfo);
}
