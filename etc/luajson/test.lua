-- import the LuaJSON module
require("LuaJSON")
local foo = json.parse([[
            {"name" : "lol", "age" : -1.5e6, "foo" : ["bar", true, null]}
            ]])
print(foo.age)    -- -1500000
print(foo.name)   -- lol
print(foo.foo[1]) -- bar
print(foo.foo[2]) -- true
print(foo.foo[3]) -- null
print(foo.foo[3] == json.null) -- true
foo.foo = "omg :D"
print(json.stringify(foo)) -- {"name":"lol",age:-1500000,"foo":"omg :D"}
