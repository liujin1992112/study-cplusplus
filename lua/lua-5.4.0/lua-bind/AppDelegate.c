#include <stdio.h>
#include "lua_extensions\lua_extensions_more.h"

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaopen_lua_extensions_more(L);
	luaL_dofile(L, "main.lua");
	lua_close(L);

	system("pause");
	return 0;
}