#include "tparser.h"
#include "tlex.h"
#include "tstring.h"
#include "tparser.h"

Proto* luaY_parser(lua_State* L, ZIO* z, const char* name) {
	LexState ls;
	FuncState fs;

	fs.f = nullptr;

	luaX_setinput(L, &ls, z, luaS_new(L, name));

	return fs.f;
}