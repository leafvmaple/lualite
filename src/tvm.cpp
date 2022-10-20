#include "tvm.h"
#include "ttable.h"

void luaV_gettable(lua_State* L, const TValue* t, TValue* key, TValue* val)
{
	Table* h = (Table*)t->value.gc;
	const TValue* res = luaH_get(h, key);
	setobj(val, res);
}

void luaV_settable(lua_State* L, const TValue* t, TValue* key, TValue* val)
{
	TValue* oldval = luaH_set(L, (Table*)t->value.gc, key);
	setobj(oldval, val);
}
