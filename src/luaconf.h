#pragma once

#define LUA_PROMPT		"> "
#define LUA_PROMPT2		">> "

#define LUA_MAXINPUT	512

inline int lua_readline(char* buff, const char* prmt) {
	fputs(prmt, stdout);
	fflush(stdout);
	return fgets(buff, LUA_MAXINPUT, stdin) != nullptr;
}

inline double lua_str2number(const char* s, char** p) {
	return strtod(s, p);
}