#pragma once

#include <limits.h>
#include <stddef.h>

#include "lua.h"

typedef unsigned char lu_byte;
typedef unsigned int  lu_int32;
typedef size_t lu_mem;

#ifndef MINSTRTABSIZE
#define MINSTRTABSIZE	32
#endif

typedef lu_int32 Instruction;

union L_Umaxalign {
    double u;
    void* s;
    long l;
};
