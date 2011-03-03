-- Fleece test file test-fleece-size.lua

print "Checking fleece.size() -- should print: 7"

package.cpath="src/?.so"
local fleece = require("fleece")

print( fleece.size({1,2,3}) )

