-- Fleece test file hello-json4.lua (not fleece)

print "Checking if JSON4 is working (for benchmarking against)"

package.path="etc/json4/?.lua"
local json4 = require("json")

t = {'ok'}
print(json4.encode(t))

