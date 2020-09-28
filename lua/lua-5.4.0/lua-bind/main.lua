--加载lua的全局函数
require("global")
json = require("cjson").new()
print("闭眼就天黑")
print(json)

local lua_bool = true
print(json.encode(lua_bool))

-- 数组类型
local lua_array = {1, 2, 3, 4, 5, 6}
print(json.encode(lua_array))

-- 数值类型
local lua_number = 6.66
print(json.encode(lua_number))

-- 字符串类型
local lua_string = "I am test1280"
print(json.encode(lua_string))

-- 对象类型
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