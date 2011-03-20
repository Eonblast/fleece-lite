-------------------------------------------------------------------------------
--- Package     : Fleece - fast Lua to JSON module                          ---
--- File        : test/bench74.lua                                          ---
--- Description : Fleece vs JSON4, luajson, Lua-Yajl: rand escapes, clocked ---
--- Version     : 0.3.1 / alpha                                             ---
--- Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     : 20 Mar 2011                                               ---
--- Changed     : 20 Mar 2011                                               ---
-------------------------------------------------------------------------------
---                                                                         ---
---  Fleece is optimized for the fastest Lua to JSON conversion and beats   ---
---  other JSON implementations by around 10 times, native Lua up to 100x.  ---
---  Please let me know about the speed you are observing.                  ---
---  hd2010@eonblast.com                                                    ---
---                                                                         ---
---  This compares Fleece to a native Lua implementation and two C libraries---
---  with Fleece' diverse escape settings switched on or off.               ---
---                                                                         ---
---  JSON4 0.9.20: (aka JSON4Lua, JSON for Lua) http://json.luaforge.net/   ---
---  LuaJSON 1.1: (aka luajsonlib) http://luaforge.net/projects/luajsonlib/ ---
---  Yajl 1.0.9: http://lloyd.github.com/yajl/                              ---
---  Lua-yajl Oct '10: http://github.com/brimworks/lua-yajl/                ---
---                                                                         ---
---  Building with Yajl is not always fun, you can safely skip this script. ---
---                                                                         ---
---  Use: lua test/bench74.lua                                              ---
---                                                                         ---
-------------------------------------------------------------------------------

print("Fleece Benchmarks for ESCAPE characters vs Json4 / native Lua and LuaJSON C lib")
print("===============================================================================")
print("A couple of random tables are created, with escape characters and speed is clocked.")
print("You should have built fleece first with 'make <PLATFORM>', ")
print("and also built luajson.so with 'make <PLATFORM>-test,")
print("And now be in the fleece root directory. You should have ")
print("built yajl into etc/yajl.")
print("There is no support in fleece to build yajl or lua-yajl.")
print("You need to get and build yajl and lua-yajl manually.")
print("Yajl 1.0.9: http://lloyd.github.com/yajl/")
print("Lua-yajl Oct '10: http://github.com/brimworks/lua-yajl/")
print("IF THIS TEST CRASHES, TRY BENCH7a, BENCH7b, BENCH4d TO SEE WHO IS AT FAULT.")

ELEMENTS = 1000
CYCLES   = 1000

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

math.randomseed( os.time() )
math.random()

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

local abc = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
		     'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'}

local esc = { '\"', '\\', '\b', '\f', '\r', '\n', '\t',
		     string.char(1), string.char(60), string.char(127) }

local chance = 20
		     
local abc_num = #abc
local esc_num = #esc

function randstr(i)
      local r = {}
      local length = math.random(1,10)
      local i
      for i = 1, length do
      	if(math.random(1,chance) == 1) then
	        r[i] = esc[math.random(1, esc_num)] -- esc!
    	else
			r[i] = abc[math.random(1, abc_num)] -- normal
      	end
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

t = nil
local function measure(prepP, prepare, actionP, action, printPrepP)

  local items  = ELEMENTS
  local cycles = CYCLES
  local clock  = os.clock

  if(printPrepP) then
    printf("%d elements in %-25s\n", items, prepP)
  end
  printf("%dx %-12s ", cycles, actionP)

  if (t == nil) then
      t = {}
	  local i = 1
	  local size = 0
	  while(i <= items) do prepare(i); i = i + 1 end
  end

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
	  printf("%dx %-12s ** sample too small, could not measure, use bench7-1k ** ", cycles, actionP)
  end
  
  return mspc, last 
end


if(_PATCH) then io.write(_PATCH) else io.write(_VERSION .. ' official') end
print(" - Fleece 0.3.1")


local function measureX(prepP, prepare, prompt1, action1, prompt2, action2, 
	prompt3, action3, prompt4, action4, prompt5, action5, prompt6, action6,
	prompt7, action7, prompt8, action8)

  print(sep)
  t = nil

	first, r1 = measure(prepP, prepare, prompt1, action1, true)
	printf("                  %.60s.. \n", r1)
	secnd, r2 = measure(prepP, prepare, prompt2, action2)
	printf("                  %.60s.. \n", r2)
	third, r3 = measure(prepP, prepare, prompt3, action3)
	printf("                  %.60s.. \n", r3)

	submeasure(prepP, prepare, prompt4, action4, first, secnd, third)
	submeasure(prepP, prepare, prompt5, action5, first, secnd, third)
	submeasure(prepP, prepare, prompt6, action6, first, secnd, third)
	submeasure(prepP, prepare, prompt7, action7, first, secnd, third)
	submeasure(prepP, prepare, prompt8, action8, first, secnd, third)

end

function submeasure(prepP, prepare, prompt, action, first, secnd, third)

	forth, r = measure(prepP, prepare, prompt, action)
	prc = prcstr(forth, first)
	printf("%3g%%, ", prc)
	prc = prcstr(forth, secnd)
	printf("%3g%% ", prc)
	prc = prcstr(forth, third)
	printf("%3g%% ", prc)
	printf("  %.60s.. \n", r)

end

print(sep)

measureX("t[i]=i",
		function(i) t[i] = i end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t, \"E0\")",
		function(i) return fleece.json(t, "E0") end,
		"fleece.json(t, \"E1\")",
		function(i) return fleece.json(t, "E1") end,
		"fleece.json(t, \"E2\")",
		function(i) return fleece.json(t, "E2") end,
		"fleece.json(t, \"E3\")",
		function(i) return fleece.json(t, "E3") end,
		"fleece.json(t, \"E4\")",
		function(i) return fleece.json(t, "E4") end
		)		

print(sep)
chance = 10
print ("Frequency of escape characters 1 in " .. chance)
		
measureX("t[i]=randstr(i)",
		function(i) t[i] = randstr(i) end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t, \"E0\")",
		function(i) return fleece.json(t, "E0") end,
		"fleece.json(t, \"E1\")",
		function(i) return fleece.json(t, "E1") end,
		"fleece.json(t, \"E2\")",
		function(i) return fleece.json(t, "E2") end,
		"fleece.json(t, \"E3\")",
		function(i) return fleece.json(t, "E3") end,
		"fleece.json(t, \"E4\")",
		function(i) return fleece.json(t, "E4") end
		)		
		
measureX("t[randstr(i)]=randstr(i)",
		function(i) t[randstr(i)] = randstr(i) end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t, \"E0\")",
		function(i) return fleece.json(t, "E0") end,
		"fleece.json(t, \"E1\")",
		function(i) return fleece.json(t, "E1") end,
		"fleece.json(t, \"E2\")",
		function(i) return fleece.json(t, "E2") end,
		"fleece.json(t, \"E3\")",
		function(i) return fleece.json(t, "E3") end,
		"fleece.json(t, \"E4\")",
		function(i) return fleece.json(t, "E4") end
		)			

print(sep)
chance = 30
print ("Frequency of escape characters 1 in " .. chance)

measureX("t[i]=randstr(i)",
		function(i) t[i] = randstr(i) end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t, \"E0\")",
		function(i) return fleece.json(t, "E0") end,
		"fleece.json(t, \"E1\")",
		function(i) return fleece.json(t, "E1") end,
		"fleece.json(t, \"E2\")",
		function(i) return fleece.json(t, "E2") end,
		"fleece.json(t, \"E3\")",
		function(i) return fleece.json(t, "E3") end,
		"fleece.json(t, \"E4\")",
		function(i) return fleece.json(t, "E4") end
		)		
		
measureX("t[randstr(i)]=randstr(i)",
		function(i) t[randstr(i)] = randstr(i) end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t, \"E0\")",
		function(i) return fleece.json(t, "E0") end,
		"fleece.json(t, \"E1\")",
		function(i) return fleece.json(t, "E1") end,
		"fleece.json(t, \"E2\")",
		function(i) return fleece.json(t, "E2") end,
		"fleece.json(t, \"E3\")",
		function(i) return fleece.json(t, "E3") end,
		"fleece.json(t, \"E4\")",
		function(i) return fleece.json(t, "E4") end
		)			

print(sep)
chance = 100
print ("Frequency of escape characters 1 in " .. chance)


measureX("t[i]=randstr(i)",
		function(i) t[i] = randstr(i) end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t, \"E0\")",
		function(i) return fleece.json(t, "E0") end,
		"fleece.json(t, \"E1\")",
		function(i) return fleece.json(t, "E1") end,
		"fleece.json(t, \"E2\")",
		function(i) return fleece.json(t, "E2") end,
		"fleece.json(t, \"E3\")",
		function(i) return fleece.json(t, "E3") end,
		"fleece.json(t, \"E4\")",
		function(i) return fleece.json(t, "E4") end
		)		
		
measureX("t[randstr(i)]=randstr(i)",
		function(i) t[randstr(i)] = randstr(i) end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t, \"E0\")",
		function(i) return fleece.json(t, "E0") end,
		"fleece.json(t, \"E1\")",
		function(i) return fleece.json(t, "E1") end,
		"fleece.json(t, \"E2\")",
		function(i) return fleece.json(t, "E2") end,
		"fleece.json(t, \"E3\")",
		function(i) return fleece.json(t, "E3") end,
		"fleece.json(t, \"E4\")",
		function(i) return fleece.json(t, "E4") end
		)			


print(sep)
chance = 1000
print ("Frequency of escape characters 1 in " .. chance)


measureX("t[i]=randstr(i)",
		function(i) t[i] = randstr(i) end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t, \"E0\")",
		function(i) return fleece.json(t, "E0") end,
		"fleece.json(t, \"E1\")",
		function(i) return fleece.json(t, "E1") end,
		"fleece.json(t, \"E2\")",
		function(i) return fleece.json(t, "E2") end,
		"fleece.json(t, \"E3\")",
		function(i) return fleece.json(t, "E3") end,
		"fleece.json(t, \"E4\")",
		function(i) return fleece.json(t, "E4") end
		)		
		
measureX("t[randstr(i)]=randstr(i)",
		function(i) t[randstr(i)] = randstr(i) end,
		"luajson.stringify(t)",
		function(i) return luajson.stringify(t) end,
		"json4.encode(t)",
		function(i) return json4.encode(t) end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end,
		"fleece.json(t, \"E0\")",
		function(i) return fleece.json(t, "E0") end,
		"fleece.json(t, \"E1\")",
		function(i) return fleece.json(t, "E1") end,
		"fleece.json(t, \"E2\")",
		function(i) return fleece.json(t, "E2") end,
		"fleece.json(t, \"E3\")",
		function(i) return fleece.json(t, "E3") end,
		"fleece.json(t, \"E4\")",
		function(i) return fleece.json(t, "E4") end
		)			



print(sep)

print("Note that fleece may list associative arrays in different order.")
print("Also note that output can be derailed by random ctrl characters.")
