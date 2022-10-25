#pragma once

#include "lopcodes.h"

struct FuncState;
union TString;

enum BinOpr {
	OPR_ADD,
	OPR_SUB,
	OPR_MUL,
	OPR_DIV,
	OPR_MOD,
	OPR_POW,
	OPR_CONCAT,
	OPR_NE,
	OPR_EQ,
	OPR_LT,
	OPR_LE,
	OPR_GT,
	OPR_GE,
	OPR_AND,
	OPR_OR,
	OPR_NOBINOPR
};

int luaK_codeABC(FuncState* fs, OpCode o, int A, int B, int C);

int luaK_stringK(FuncState* fs, TString* s);