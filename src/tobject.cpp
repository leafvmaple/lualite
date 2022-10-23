#include "tobject.h"

bool ttisnumber(TValue* obj){
    return obj->tt == LUA_TNUMBER;
}
