-- Fleece test file test-fleece-json.lua (the main thing in all this really)

io.write("This tests, wether json() responds to a most simple call:\n")
io.write("Calling fleece.json({'ok'})  -- should print [\"ok\"]: ")

package.cpath="src/?.so"
local fleece = require("fleece")

print( fleece.json({"ok"}) )
