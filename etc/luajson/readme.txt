LuaJSON_lib

Version 1.1, 14 November 2010
by Nathaniel Musgrove (proton.zero@gmail.com)

   LuaJSON_lib provides two functions, json.parse() and
   json.stringify(), and one constant, json.null.

   json.parse() parses any single JSON value (object, array, string,
   number, true, false, null) into an appropriate Lua data structure.

      Objects are parsed as tables with string-keys.  Arrays are parsed
      as tables with number-keys.  Strings are parsed as strings,
      numbers as numbers, true as true, false as false, and null is
      parsed as json.null, a constant used solely for the purpose of
      representing JSON's null value.

   json.stringify() takes a single Lua value (which may be complex, such
   as nested tables) and outputs a JSON-formatted string representing
   that value.

      Tables are stringified as arrays if table[1] ~= nil, and
      stringified as objects otherwise (only table elements with keys
      that are strings will be put into the JSON object).  Strings are
      stringified as strings, escaping backslashes, double-quotation
      marks, form feeds, line feeds, carriage returns, tabs, and
      backspace-characters, and unicode-escaping any other character
      less than 0x20.  Numbers are stringified as numbers, booleans as
      booleans, and json.null as null.  Anything other than tables,
      strings, numbers, booleans, and json.null will produce an invalid
      JSON string (except nil, which produces an empty string).


Example

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
