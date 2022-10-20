#include "tlex.h"
#include "tstring.h"

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

void luaX_setinput(lua_State* L, LexState* ls, ZIO* z, TString* source) {
    ls->L = L;
    ls->z = z;
    ls->fs = nullptr;
	ls->linenumber = 1;
    ls->source = source;
    ls->buff.clear();
    next(ls);
}
