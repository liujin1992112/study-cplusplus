// lua-test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <lua.hpp>

int _tmain(int argc, _TCHAR* argv[])
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L, "main.lua");
	lua_close(L);
	system("pause");
	return 0;
}

