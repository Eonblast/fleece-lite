-------------------------------------------------------------------------------
--- Package     : Fleece - fast Lua to JSON module                          ---
--- File        : test/bench-yajl.lua                                       ---
--- Description : Lua-Yajl benchmarks: random tables, speed clocked         ---
--- Version     : 0.2.2 / alpha                                             ---
--- Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     :    Feb 2011                                               ---
--- Changed     : 02 Mar 2011                                               ---
-------------------------------------------------------------------------------
---                                                                         ---
---  This runs Lua-Yajl, a streaming C implementation of JSON.              ---
---                                                                         ---
---  Yajl 1.0.9: http://lloyd.github.com/yajl/                              ---
---  Lua-yajl Oct '10: http://github.com/brimworks/lua-yajl/                ---
---                                                                         ---
---  Building with Yajl is not always fun, you can safely skip this.        ---
---                                                                         ---
---  Use: lua test/bench1-yajl.lua                                          ---
---                                                                         ---
-------------------------------------------------------------------------------

print("Fleece Benchmarks: Lua-Yajl only")
print("=========================================================")
print("A couple of random tables are created and speed is clocked.")
print("Run this in the main fleece dir and have yajl built into etc/yajl.")
print("There is no support in fleece to build yajl or lua-yajl,")
print("you need to get and build them manually.")
print("Yajl 1.0.9: http://lloyd.github.com/yajl/")
print("Lua-yajl Oct '10: http://github.com/brimworks/lua-yajl/")

ELEMENTS = 1000
CYCLES   = 100

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

local function measure1(prepP, prepare, p1, a1)
	
	print(sep)

	first, r1 = measure(prepP, prepare, p1, a1, true)
	printf("         %.20s.. \n", r1)
	
end

measure1("t[i]=i",
		function(i) t[i] = i end,
		"yajl.to_string(t)",
		function(i) return yajl.to_string(t) end
		)

