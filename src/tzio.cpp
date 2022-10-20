#include "tzio.h"
#include <malloc.h>

int char2int(char c) {
	return static_cast<int>(c);
}

int zgetc(ZIO* z) {
	if (z->n-- > 0)
		return char2int(*z->p++);
	return luaZ_fill(z);
}

void luaZ_init(lua_State* L, ZIO* z, lua_Reader reader, void* data) {
	z->L = L;
	z->reader = reader;
	z->data = data;
	z->n = 0;
	z->p = nullptr;
}

int luaZ_fill(ZIO* z) {
	size_t size = 0;
	const char* buff = nullptr;

	buff = z->reader(z->L, z->data, &size);
	if (!buff || !size)
		return EOZ;
	z->n = size - 1;
	z->p = buff;
	return (*(z->p++));
}

