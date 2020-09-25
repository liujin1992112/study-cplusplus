#include <stdio.h>
#include "lua_extensions\lua_extensions_more.h"

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaopen_lua_extensions_more(L);
	int nRet = luaL_dofile(L, "main.lua");
	if (nRet != LUA_OK)
	{
		const char *err = lua_tostring(L, -1); //加载失败，会把错误信息压入栈顶
		printf("-------loadfile error = %s\n", err);
	}
	lua_close(L);

	system("pause");
	return 0;
}