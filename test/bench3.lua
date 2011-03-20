-------------------------------------------------------------------------------
--- Package     : Fleece - fast Lua to JSON module                          ---
--- File        : test/bench3.lua                                           ---
--- Description : Fleece vs JSON4 and luajson: random tables, speedcked     ---
--- Version     : 0.3.1 / alpha                                             ---
--- Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     :    Feb 2011                                               ---
--- Changed     : 02 Mar 2011                                               ---
-------------------------------------------------------------------------------
---                                                                         ---
---  Fleece is optimized for the fastest Lua to JSON conversion and beats   ---
---  other JSON implementations by around 10 times, native Lua up to 100x.  ---
---  Please let me know about the speed you are observing.                  ---
---  hd2010@eonblast.com                                                    ---
---                                                                         ---
---  This compares Fleece to a native Lua implementation and a C library:   ---
---                                                                         ---
---  JSON4 0.9.50: (aka JSON4Lua, JSON for Lua) http://json.luaforge.net/   ---
---  LuaJSON 1.1: (aka luajsonlib) http://luaforge.net/projects/luajsonlib/ ---
---                                                                         ---
---  Use: lua test/bench3.lua                                               ---
---                                                                         ---
-------------------------------------------------------------------------------

print("Fleece Benchmarks vs Json4 / native Lua and LuaJSON C lib")
print("=========================================================")
print("A couple of random tables are created and speed is clocked.")
print("You should have built fleece first with 'make <PLATFORM>', ")
print("and also built luajson.so with 'make <PLATFORM>-test,")
print("and now be in the fleece root directory.")
print("IF THIS TEST CRASHES, TRY BENCH3a & BENCH3b TO SEE WHO DOES.")

ELEMENTS = 1000
CYCLES   = 100

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


sep = "---------------------------------------------------------------------------------"

printcol = 0
function printf(...)
	s = string.format(...)
	if(s:find("\n") or s:len() == 0) then printcol = 0 else printcol = printcol + s:len() end
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
 
 
function prcstr(part, base)
    if base == 0 or base == nil or part == nil then return 0 end
    x = math.floor(part / base * 100)
    if(x <= 2) then
        x = math.floor(part / base * 1000) / 10
    end
    return x
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
  local last = nil
  local tm = clock()
  while(i <= cycles) do last = action(i); i = i + 1 end
  tm = clock() - tm
  if tm ~= 0 then
  	 mspc= nanosecs_per(tm, cycles * items)
  	 tab(27)
  	 printf("%10.0fns/element ", mspc)
  else
	  mspc = nil
	  printf("%dx %-12s ** sample too small, could not measure, use bench3-1k ** ", cycles, actionP)
  end
  
  return mspc, last 
end
if(_PATCH) then io.write(_PATCH) else io.write(_VERSION .. ' official') end
print(" - Fleece 0.3.1")


local function measure3(prepP, prepare, prompt1, action1, prompt2, action2, prompt3, action3)

  print(sep)

	first, r1 = measure(prepP, prepare, prompt1, action1, true)
	printf("            %.20s.. \n", r1)
	secnd, r2 = measure(prepP, prepare, prompt2, action2)
	printf("            %.20s.. \n", r2)
	third, r3 = measure(prepP, prepare, prompt3, action3)
	
	prc = prcstr(third, first)
	printf("%3g%%, ", prc)
	prc = prcstr(third, secnd)
	printf("%3g%%  %.20s.. \n", prc, r3)

end


measure3("t[i]=i",
		function(i) t[i] = i end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"fleece.json(t)",
		function(i) return fleece.json(t) end
		)

measure3("t['x'..i]=i",
		function(i) t['x'..i] = i end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"fleece.json(t)",
		function(i) return fleece.json(t) end
		)		

measure3("t[i]=randstr(i)",
		function(i) t[i] = randstr(i) end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"fleece.json(t)",
		function(i) return fleece.json(t) end
		)		
		
measure3("t[randstr(i)]=randstr(i)",
		function(i) t[randstr(i)] = randstr(i) end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"fleece.json(t)",
		function(i) return fleece.json(t) end
		)			
print(sep)

print("Note that fleece may list associative arrays in different order.")

