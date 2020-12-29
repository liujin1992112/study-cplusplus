#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include <lua.hpp>
#include "lua_extensions\lua_extensions_more.h"

/**
 * ��C++���鴫��lua��table
 */
void pushVecIntToTable(lua_State* L, const std::vector<int>& v)
{
	int n = lua_gettop(L);	/* �����ĸ��� */
	printf("stack size:%d\n", n);
	lua_newtable(L);
	for (unsigned int i = 0; i < v.size(); i++)
	{
		lua_pushinteger(L, v[i]);

		//����-2��λ��ΪĿ��table,��i+1����Ϊlua�������±��1��ʼ
		//�ȼ��� t[i] = v �� ����� t ��ָ�����������ı� �� v ��ջ����ֵ��
		//��������Ὣֵ����ջ�� ��ֵ��ֱ�ӵģ������ᴥ��Ԫ������
		lua_rawseti(L, -2, i + 1);
	}
	n = lua_gettop(L);	/* �����ĸ��� */
	printf("pushVecIntToTable --> stack size:%d\n", n);
}


void pushMapToTable(lua_State* L, const std::map<std::string, std::string>& m)
{
	int n = lua_gettop(L);	/* �����ĸ��� */
	printf("stack size:%d\n", n);
	lua_newtable(L);
	for (auto it = m.begin(); it != m.end(); ++it)
	{
		lua_pushstring(L, it->second.c_str());
		lua_setfield(L, -2, it->first.c_str());
	}
	n = lua_gettop(L);	/* �����ĸ��� */
	printf("pushMapToTable --> stack size:%d\n", n);
}

void iterAndPrintArray(lua_State* L, int index)
{
	printf("������ͨ��������table:\n");
	int i = 1;
	while (true)
	{
		//��t[n]��ֵѹջ�������t��ָ�����������ı�����һ��ֱ�ӷ��ʣ�����˵�������ᴥ��Ԫ������������ջֵ�����͡�
		lua_rawgeti(L, index, i);
		if (lua_isnil(L, -1))
		{
			break;
		}
		printf("key %d is %s\n", i++, lua_tostring(L, -1));

		//��ջ�е���1��Ԫ��
		lua_pop(L, 1);
	}
}

/**
 *	C++�е���lua��api�������ӵ�lua table,
 *  �ο��ٷ��ĵ�:
 *  https://www.runoob.com/manual/lua53doc/manual.html#lua_next
 */
void iterAndPrintTable(lua_State* L, int index)
{
	printf("����������table:\n");
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
		const char *err = lua_tostring(L, -1); //����ʧ�ܣ���Ѵ�����Ϣѹ��ջ��
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