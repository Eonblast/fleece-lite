-- Fleece test file test-fleece-hello.lua (a dummy function)

io.write("This tests, wether the Fleece library can be loaded at all:\n")
io.write("Calling fleece.hello() -- should print: Hello LuaJSON World!: ")

package.cpath="src/?.so"
local fleece = require("fleece")

fleece.hello()
