-------------------------------------------------------------------------------
--- Package     : Fleece - fast Lua to JSON module                          ---
--- File        : test/bench4.lua                                           ---
--- Description : Fleece vs JSON4, luajson and Lua-Yajl                     ---
--- Version     : 0.2.2 / alpha                                             ---
--- Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     :    Feb 2011                                               ---
--- Changed     : 02 Mar 2011                                               ---
-------------------------------------------------------------------------------
---                                                                         ---
---  Fleece is optimized for the fastest Lua to JSON conversion and beats   ---
---  other JSON implementations by around 10 times, native Lua around 100.  ---
---  Please let me know about the speed you are observing.                  ---
---  hd2010@eonblast.com                                                    ---
---                                                                         ---
---  This compares Fleece to JSON4, LuaJSON and Lua-Yajl:                   ---
---                                                                         ---
---  JSON4 0.9.50: (aka JSON4Lua, JSON for Lua) http://json.luaforge.net/   ---
---  LuaJSON 1.1: (aka luajsonlib) http://luaforge.net/projects/luajsonlib/ ---
---  Yajl 1.0.9: http://lloyd.github.com/yajl/                              ---
---  Lua-yajl Oct '10: http://github.com/brimworks/lua-yajl/                ---
---                                                                         ---
---  Building with Yajl is not always fun, you can safely skip this.        ---
---                                                                         ---
---  Use: lua test/bench4.lua                                               ---
---                                                                         ---
-------------------------------------------------------------------------------

print("Fleece Benchmarks vs Json4, LuaJSON C lib and Lua-Yajl C lib")
print("============================================================")
print("A couple of random tables are created and speed is clocked.")
print("Building with Yajl is not always fun, you can safely skip this.")
print("You should have built fleece first with 'make <PLATFORM>', and also")
print("built luajson.so with 'make <PLATFORM>-test, and now be in the")
print("fleece root directory. You should have built yajl into etc/yajl.")
print("There is no support in fleece to build yajl or lua-yajl.")
print("You need to get and build yajl and lua-yajl manually.")
print("Yajl 1.0.9: http://lloyd.github.com/yajl/")
print("Lua-yajl Oct '10: http://github.com/brimworks/lua-yajl/")
print("LUAJSON (NOT FLEECE) CAN CRASH THIS TEST. SEE BENCH3a & BENCH3b.")

ELEMENTS = 1000
CYCLES   = 10

package.cpath="src/?.so"
fleece = require("fleece")
package.path="etc/json4/?.lua"
json4 = require("json")
package.cpath="etc/luajson/?.so"
luajson = require("luajson")

-- luajson stuff
local base = _G
local json = luajson
json.null = {_mt = {__tostring = function () return "null" end, __call = function () return "null" end}}
base.setmetatable(json.null, json.null._mt)

-- luayajl stuff
package.cpath = "etc/yajl/?.so" 
local yajl  = require("yajl")

sep = "---------------------------------------------------------------------------------"

printcol = 0
function printf(...)
	s = string.format(...)
	if(s:find("\n")) then printcol = 0 else printcol = printcol + s:len() end
    io.write(s)
end

function tab(x)
	while(printcol < x) do io.write(" "); printcol = printcol + 1 end
end

function nanosecs_per(time, per)
	return time * 1000000000 / per
end

function microsecs_per(time, per)
	return time * 1000000 / per
end

local abc = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'}
function randstr(i)
      local r = {}
      local length = math.random(1,20)
      local i
      for i = 1, length do
         r[i] = abc[math.random(1, 26)]
      end

      return table.concat(r)
 end

local t = {}
local function measure(prepP, prepare, actionP, action, printPrepP)

  local items  = ELEMENTS
  local cycles = CYCLES
  local clock  = os.clock

  if(printPrepP) then
    printf("%d elements in %-25s\n", items, prepP)
  end
  printf("%dx %-12s ", cycles, actionP)

  t = {}
  local i = 1
  local size = 0
  while(i <= items) do prepare(i); i = i + 1 end

  i = 1
  last = nil
  local tm = clock()
  while(i <= cycles) do last = action(i); i = i + 1 end
  tm = clock() - tm
  if tm ~= 0 then
  	 mspc= nanosecs_per(tm, cycles * items)
  	 tab(27)
  	 printf("%10.0fns/element ", mspc)
  else
	  mspc = nil
	  printf("%dx %-12s sample too small, could not measure ", cycles, actionP)
  end
  
  return mspc, last 
end
if(_PATCH) then print(_PATCH) else print(_VERSION .. ' official') end

local function measure4(prepP, prepare, p1, a1, p2, a2, p3, a3, p4, a4)
	
	print(sep)

	first, r1 = measure(prepP, prepare, p1, a1, true)
	printf("         %.20s.. \n", r1)
	secnd, r2 = measure(prepP, prepare, p2, a2)
	printf("         %.20s.. \n", r2)
	third, r3 = measure(prepP, prepare, p3, a3)
	printf("         %.20s.. \n", r3)
	forth, r4 = measure(prepP, prepare, p4, a4)
	
	if(first and forth) then prc = math.floor(forth / first * 100) else prc = "-" end
	printf("%2d%% ", prc)
	if(secnd and forth) then prc = math.floor(forth / secnd * 100) else prc = "-" end
	printf("%2d%% ", prc)
	if(third and forth) then prc = math.floor(forth / third * 100) else prc = "-" end
	printf("%2d%% ", prc)
	printf(" %.20s.. \n", r4)
	
end

measure4("t[i]=i",
		function(i) t[i] = i end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t)",
		function(i) return fleece.json(t) end
		)

measure4("t['x'..i]=i",
		function(i) t['x'..i] = i end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t)",
		function(i) return fleece.json(t) end
		)		

measure4("t[i]=randstr(i)",
		function(i) t[i] = randstr(i) end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t)",
		function(i) return fleece.json(t) end
		)		
		
measure4("t[randstr(i)]=randstr(i)",
		function(i) t[randstr(i)] = randstr(i) end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t)",
		function(i) return fleece.json(t) end
		)			
print(sep)
print("Note that fleece may list associative arrays in different order.")

