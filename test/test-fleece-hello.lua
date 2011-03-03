-- Fleece test file test-fleece-hello.lua (a dummy function)

print "Checking fleece.hello() -- should print: Hello LuaJSON World!"

package.cpath="src/?.so"
local fleece = require("fleece")

fleece.hello()
