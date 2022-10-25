#include "tobject.h"

bool ttisnumber(TValue* obj){
    return obj->tt == LUA_TNUMBER;
}

void setnilvalue(TValue* obj) {
	obj->tt = LUA_TNIL;
}

void setnvalue(TValue* obj, const lua_Number n) {
	obj->value.n = n;
	obj->tt = LUA_TNUMBER;
}

void setpvalue(TValue* obj, void* p) {
	obj->value.p = p;
	obj->tt = LUA_TLIGHTUSERDATA;
}

void setbvalue(TValue* obj, const bool b) {
	obj->value.b = b;
	obj->tt = LUA_TBOOLEAN;
}

void setsvalue(TValue* obj, const TString* s) {
	obj->value.gc = (GCObject*)s;
	obj->tt = LUA_TSTRING;
}

void sethvalue(TValue* obj, const Table* h) {
	obj->value.gc = (GCObject*)h;
	obj->tt = LUA_TTABLE;
}

void setclvalue(TValue* obj, const Closure* cl) {
	obj->value.gc = (GCObject*)cl;
	obj->tt = LUA_TFUNCTION;
}

void setptvalue(TValue* obj, const Proto* pt) {
	obj->value.gc = (GCObject*)pt;
	obj->tt = LUA_TPROTO;
}

void setobj(TValue* desc, const TValue* src) {
	desc->value.gc = src->value.gc;
	desc->tt = src->tt;
}