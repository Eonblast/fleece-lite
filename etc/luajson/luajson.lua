require("LuaJSON_lib", package.seeall)

local base = _G
local json = json
module("json")

json.null = {_mt = {__tostring = function () return "null" end, __call = function () return "null" end}}
base.setmetatable(json.null, json.null._mt)

return json
