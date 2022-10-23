#pragma once

#include "tobject.h"
#include "tzio.h"

#define FIRST_RESERVED	257

struct FuncState;

enum RESERVED {
    /* terminal symbols denoted by reserved words */
    TK_AND = FIRST_RESERVED,
    TK_BREAK,
    TK_DO,
    TK_ELSE,
    TK_ELSEIF,
    TK_END,
    TK_FALSE,
    TK_FOR,
    TK_FUNCTION,
    TK_IF,
    TK_IN,
    TK_LOCAL,
    TK_NIL,
    TK_NOT,
    TK_OR,
    TK_REPEAT,
    TK_RETURN,
    TK_THEN,
    TK_TRUE,
    TK_UNTIL,
    TK_WHILE,
    /* other terminal symbols */
    TK_CONCAT,
    TK_DOTS,
    TK_EQ,
    TK_GE,
    TK_LE,
    TK_NE,
    TK_NUMBER,
    TK_NAME,
    TK_STRING,
    TK_EOS
};

/* number of reserved words */
#define NUM_RESERVED	(TK_WHILE - FIRST_RESERVED + 1)

union SemInfo {
    lua_Number r;
    TString* ts;
};

struct Token {
    int token;
    SemInfo seminfo;
};

struct LexState {
    int current;
    int linenumber;
    int lastline;
    Token t;            // µ±Ç°Token
    FuncState* fs;
    lua_State* L;
    ZIO* z;
    std::string buff;
    TString* source;
};

void luaX_init(lua_State* L);
TString* luaX_newstring(LexState* ls, const char* str, size_t l);
void luaX_setinput(lua_State* L, LexState* ls, ZIO* z, TString* source);
void luaX_next(LexState* ls);