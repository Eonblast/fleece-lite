-- Fleece test file hello-luajson.lua (not fleece)

print "Checking if LuaJSON is working (for benchmarking against)"

package.cpath="etc/luajson/?.so"
local luajson = require("luajson")

-- luajson.null = {_mt = {__tostring = function () return "null" end, __call = function () return "null" end}}
-- base.setmetatable(luajson.null, luajson.null._mt)

t = {'ok'}
print(luajson.stringify(t))

