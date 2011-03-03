-- Fleece test file test-fleece-json.lua (the main thing in all this really)

print "Checking fleece.json()  -- should print: [\"ok\"]"

package.cpath="src/?.so"
local fleece = require("fleece")

print( fleece.json({"ok"}) )
