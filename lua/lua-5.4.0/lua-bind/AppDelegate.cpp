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


void operateLuaTable(lua_State* L)
{
	std::vector<int> v = { 10, 20, 30, 40, 50, 60 };
	std::map<std::string, std::string> m;
	m["age"] = "19";
	m["name"] = "liujin";
	m["sex"] = "1";

	pushVecIntToTable(L, v);
	iterAndPrintArray(L, -1);

	pushMapToTable(L, m);
	iterAndPrintTable(L, -2);
}

//c++调用lua中的全局add函数
void cplusplusCallLuaFunc(lua_State* L)
{	
	//载入test.lua脚本
	int nRet = luaL_dofile(L, "test1.lua");
	if (nRet != LUA_OK)
	{
		const char *err = lua_tostring(L, -1); //加载失败，会把错误信息压入栈顶
		printf("loadfile error = %s\n", err);
		lua_pop(L, 1);
	}
	else
	{
		//获取全局变量add,并压入栈中,add是test.lua中定义的全局函数
		int nargs = 2;
		lua_getglobal(L, "add");
		//压入参数
		lua_pushinteger(L, 1);
		lua_pushinteger(L, 2);
		//参数:lua_State*,参数个数,返回值个数,不设置错误处理回调
		nRet = lua_pcall(L, nargs, 1, 0);

		//lua_tointeger采用负向索引的方式,从栈顶到栈底方式访问
		printf("add result:%d\n", lua_tointeger(L, -1));

		//弹出返回值
		lua_pop(L, 1);
	}
}

/**
 * 任何在Lua中注册的函数必须有同样的原型,这个原型就是lua.h中lua_CFunction,定义如下
 * typedef int (*lua_CFunction)(lua_State* L)
 * 返回值:表示参数个数
 */
int sub_for_lua(lua_State*L)
{
	//lua_tointeger采用正向索引的方式,从栈底到栈顶访问
	double a = lua_tointeger(L, 1);
	double b = lua_tointeger(L, 2);
	lua_pushnumber(L, a - b);//将计算结果压缩栈顶
	return 1;
}

void luaCallCPlusplus(lua_State*L)
{
	//lua_register:
	//相当于
	//-->lua_pushcfunction(L, sub_for_lua) //将sub_for_lua压入栈顶
	//-->lua_setglobal(L, "sub")		   //将sub_for_lua弹出栈顶,并设置给全局变量sub

	//lua_setglobal 从堆栈上弹出一个值，并将其设为全局变量 name 的新值
	lua_register(L, "sub", sub_for_lua);

	//c++模拟调用lua中全局变量(可以采用lua脚本的方式调用)
	lua_getglobal(L, "sub");
	lua_pushinteger(L, 100);
	lua_pushinteger(L, 10);
	lua_pcall(L, 2, 1, 0);
	printf("sub result:%d\n", lua_tointeger(L, -1));
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

	operateLuaTable(L);

	cplusplusCallLuaFunc(L);
	luaCallCPlusplus(L);
	
	lua_close(L);

	system("pause");
	return 0;
}