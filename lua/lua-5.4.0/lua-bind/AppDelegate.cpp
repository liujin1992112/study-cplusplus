#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include <lua.hpp>
#include "lua_extensions\lua_extensions_more.h"

/**
 * 将C++数组传入lua中table
 */
void pushVecIntToTable(lua_State* L, const std::vector<int>& v)
{
	int n = lua_gettop(L);	/* 参数的个数 */
	printf("stack size:%d\n", n);
	lua_newtable(L);
	for (unsigned int i = 0; i < v.size(); i++)
	{
		lua_pushinteger(L, v[i]);

		//索引-2的位置为目标table,而i+1是因为lua的数组下标从1开始
		//等价于 t[i] = v ， 这里的 t 是指给定索引处的表， 而 v 是栈顶的值。
		//这个函数会将值弹出栈。 赋值是直接的；即不会触发元方法。
		lua_rawseti(L, -2, i + 1);
	}
	n = lua_gettop(L);	/* 参数的个数 */
	printf("pushVecIntToTable --> stack size:%d\n", n);
}


void pushMapToTable(lua_State* L, const std::map<std::string, std::string>& m)
{
	int n = lua_gettop(L);	/* 参数的个数 */
	printf("stack size:%d\n", n);
	lua_newtable(L);
	for (auto it = m.begin(); it != m.end(); ++it)
	{
		lua_pushstring(L, it->second.c_str());
		lua_setfield(L, -2, it->first.c_str());
	}
	n = lua_gettop(L);	/* 参数的个数 */
	printf("pushMapToTable --> stack size:%d\n", n);
}

void iterAndPrintArray(lua_State* L, int index)
{
	printf("遍历普通的数组类table:\n");
	int i = 1;
	while (true)
	{
		//把t[n]的值压栈，这里的t是指给定索引处的表。这是一次直接访问；就是说，它不会触发元方法。返回入栈值的类型。
		lua_rawgeti(L, index, i);
		if (lua_isnil(L, -1))
		{
			break;
		}
		printf("key %d is %s\n", i++, lua_tostring(L, -1));

		//从栈中弹出1个元素
		lua_pop(L, 1);
	}
}

/**
 *	C++中调用lua的api遍历复杂的lua table,
 *  参考官方文档:
 *  https://www.runoob.com/manual/lua53doc/manual.html#lua_next
 */
void iterAndPrintTable(lua_State* L, int index)
{
	printf("遍历复杂类table:\n");
	lua_pushnil(L);
	while (lua_next(L, index) !=0)
	{
		printf("key %d is %s\t", lua_tostring(L, -2), lua_tostring(L, -1));
		printf("key-type %s; value-type %s\n", lua_typename(L, lua_type(L, -2)), lua_typename(L, lua_type(L, -1)));
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
}


int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaopen_lua_extensions_more(L);
	int nRet = luaL_dofile(L, "main.lua");
	if (nRet != LUA_OK)
	{
		const char *err = lua_tostring(L, -1); //加载失败，会把错误信息压入栈顶
		printf("loadfile error = %s\n", err);
		lua_pop(L, 1);
	}

	std::vector<int> v = { 10, 20, 30, 40, 50, 60 };
	std::map<std::string, std::string> m;
	m["age"] = "19";
	m["name"] = "liujin";
	m["sex"] = "1";

	pushVecIntToTable(L, v);
	iterAndPrintArray(L, -1);
 

	pushMapToTable(L, m);
	iterAndPrintTable(L, -2);

	lua_close(L);

	system("pause");
	return 0;
}