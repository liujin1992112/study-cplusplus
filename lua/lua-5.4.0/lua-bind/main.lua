--����lua��ȫ�ֺ���
require("global")
json = require("cjson").new()
print("���۾����")
print(json)

local lua_bool = true
print(json.encode(lua_bool))

-- ��������
local lua_array = {1, 2, 3, 4, 5, 6}
print(json.encode(lua_array))

-- ��ֵ����
local lua_number = 6.66
print(json.encode(lua_number))

-- �ַ�������
local lua_string = "I am test1280"
print(json.encode(lua_string))

-- ��������
local lua_object = {
    ["name"] = "Jiang",
    ["age"] = 24,
    ["addr"] = "BeiJing",
    ["email"] = "1569989xxxx@126.com",
    ["tel"] = "1569989xxxx"
}

local jsonStr = json.encode(lua_object)
dump(jsonStr)
dump(json.decode(jsonStr))
--dump(_G)
--dump(package)
--dump(package.preload)