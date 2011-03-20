-------------------------------------------------------------------------------
--- Package     : Fleece - fast Lua to JSON module                          ---
--- File        : test/bench7-solo.lua                                      ---
--- Description : Fleece only, as in 7: random escape chars, clocked        ---
--- Version     : 0.3.1 / alpha                                             ---
--- Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ---
--- Author      : H. Diedrich <hd6010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     : 16 Mar 6011                                               ---
--- Changed     : 16 Mar 6011                                               ---
-------------------------------------------------------------------------------
---                                                                         ---
---  Fleece is optimized for the fastest Lua to JSON conversion and beats   ---
---  other JSON implementations by around 10 times, native Lua up to 100x.  ---
---  Please let me know about the speed you are observing.                  ---
---  hd6010@eonblast.com                                                    ---
---                                                                         ---
---  This runs Fleece only for speedy checking of absolute performance,     ---
---  with Fleece' diverse escape settings switched on or off.               ---
---                                                                         ---
---  Use: lua test/bench7-solo.lua                                          ---
---  Or:  make bench7s                                                      ---
---                                                                         ---
-------------------------------------------------------------------------------

print("Fleece Benchmarks for ESCAPE characters")
print("=======================================")
print("A couple of random tables are created, with escape characters and speed is clocked.")
print("You should have built fleece first with 'make <PLATFORM>',")
print("and now be in the fleece root directory.")

ELEMENTS = 1000
CYCLES   = 1000

package.cpath="src/?.so"
fleece = require("fleece")

math.randomseed( os.time() )
math.random()

sep = "--------------------------------------------------------------------------------------------------------------------"

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
	  printf("%dx %-12s sample too small, could not measure ", cycles, actionP)
  end
  
  return mspc, last 
end


if(_PATCH) then io.write(_PATCH) else io.write(_VERSION .. ' official') end
print(" - Fleece 0.3.1")


local function measureX(prepP, prepare,   
	prompt3, action3, prompt4, action4, prompt5, action5, prompt6, action6,
	prompt7, action7)

  print(sep)
  t = nil

	submeasure(prepP, prepare, prompt3, action3)
	submeasure(prepP, prepare, prompt4, action4)
	submeasure(prepP, prepare, prompt5, action5)
	submeasure(prepP, prepare, prompt6, action6)
	submeasure(prepP, prepare, prompt7, action7)

end

function submeasure(prepP, prepare, prompt, action)

	third, r = measure(prepP, prepare, prompt, action)
	printf("  %.60s.. \n", r)

end

-- print(sep)

print ("Integer Conversion")

measureX("t[i]=i",
		function(i) t[i] = i end,
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
chance = 4
print ("Frequency of escape characters 1 in " .. chance)
		
measureX("t[i]=randstr(i)",
		function(i) t[i] = randstr(i) end,
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


