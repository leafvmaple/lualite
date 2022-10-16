#include "tlex.h"
#include "tstring.h"

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
        TString* ts = _luaS_new(L, luaX_tokens[i]);
        ts->tsv.reserved = (lu_byte)(i + 1);
    }
}