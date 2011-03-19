-------------------------------------------------------------------------------
--- Package     : Fleece - fast Lua to JSON module                          ---
--- File        : test-fleece-nonstop.lua                                   ---
--- Description : test: endless short, random, nested tables                ---
--- Version     : 0.3.0 / alpha                                             ---
--- Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     : 08 Mar 2011                                               ---
--- Changed     : 08 Mar 2011                                               ---
-------------------------------------------------------------------------------
---                                                                         ---
-------------------------------------------------------------------------------

print("Fleece Never Ending Short, Nested, Random Tests")
print("-----------------------------------------------")
print("Random tables (strict arrays) are created and converted.")
print("You should build fleece first with 'make PLATFORM' and now be in ../src.")
print("The dump printed for comparison uses a native Lua function by Roberto.")

package.cpath="src/?.so"
fleece = require("fleece")

function printf(...)
  io.write(string.format(...))
end

function nanosecs_per(time, cycles)
	return time * 1000000000 / cycles
end

-- make interesting tables to test for fleece
-- the lua random generator produces the same results with every next start

-----------------------------------------------------------------------
-- randomizing
-----------------------------------------------------------------------
abc={'a','e','i','o','u', ' ', '"', '/', '-', 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','q','x','y','z'}
abclen = #abc

math.randomseed( tonumber(tostring(os.time()):reverse():sub(1,6)) )

escflags = {'E','E0','E1','E2','E3','E4'}
escflagsnum = #escflags

function random_esc_flag()
	
	local r = math.random(1, escflagsnum)
	print("Fleece escape flag: " .. escflags[r])
	return escflags[r]

end

-- random string generator
function rndstr(min, max)
	
	local _abc=abc
	local _abclen=abclen
	
	local s = {}
	local l = math.random(min, max)
	
	local i=1
	while(i<=l) do
		s[i] = _abc[math.random(_abclen)]
		i = i + 1
	end
	
	return table.concat(s)

end

function rndtbl(maxelements, maxdepth, depth, skip)

	local t = {}
	if (maxelements == nil) then maxelements = 10 end
	if (maxdepth == nil) then maxdepth = 3 end
	if (depth == nil) then depth = 0 end
	local l = math.random(0,maxelements)

	local i = 1
	local off = 0
	while(i<=l) do

		-- make holes (1:5 chance)
		if(skip ~= nil and math.random(1,5) == 1) then
			off = off + math.random(1,20)
		end
		key = i + off

		-- make string keys (1:5 chance)
		if(skip ~= nil and math.random(1,5) == 1) then
			key = rndstr(1,7)
		end

		local r = math.random(1,5)

		if (r==1) then
			t[key] = rndstr(1,20)

		elseif(r==2) then
			t[key] = math.random(-10000000, 10000000)

		elseif(r==3) then
			t[key] = math.random(-10000000, 10000000) / math.random(1, 1000000000)

		elseif(r==4) then
			if (depth < maxdepth) then
				t[key] = rndtbl(maxelements / 2, maxdepth, depth+1)
			else
				t[key] = {}
			end 
			
		elseif(r==5) then
			if (math.random(1,2) == 1) then
				t[key] = true
			else
				t[key] = false
			end
		end
		
		-- print("D" .. depth .. " #" ..i ..": " .. table.val_to_str(t[i]))
		i = i + 1

	end

	return t	
	
end

-----------------------------------------------------------------------
-- mini json stringify for output
-----------------------------------------------------------------------
-- from http://lua-users.org/wiki/TableUtils
function table.val_to_str ( v )
  if "string" == type( v ) then
    v = string.gsub( v, "\n", "\\n" )
    if string.match( string.gsub(v,"[^'\"]",""), '^"+$' ) then
      return "'" .. v .. "'"
    end
    return '"' .. string.gsub(v,'"', '\\"' ) .. '"'
  else
    return "table" == type( v ) and table.tostring( v ) or
      tostring( v )
  end
end

-- from http://lua-users.org/wiki/TableUtils
function table.key_to_str ( k )
  if "string" == type( k ) and string.match( k, "^[_%a][_%a%d]*$" ) then
    return k
  else
    return "[" .. table.val_to_str( k ) .. "]"
  end
end

-- from http://lua-users.org/wiki/TableUtils
function table.tostring( tbl )
  local result, done = {}, {}
  for k, v in ipairs( tbl ) do
    table.insert( result, table.val_to_str( v ) )
    done[ k ] = true
  end
  for k, v in pairs( tbl ) do
    if not done[ k ] then
      table.insert( result,
        table.key_to_str( k ) .. "=" .. table.val_to_str( v ) )
    end
  end
  return "{" .. table.concat( result, "," ) .. "}"
end

loopcount = 0
while true do
	
	  loopcount = loopcount +1

	  -----------------------------------------------------------------------
	  -- test calls
	  -----------------------------------------------------------------------
	  function donk(t) print( table.val_to_str( t ) ) end
	  sep = "---------------------------------------------------------------"
	  sep2= "***************************************************************"
	  -----------------------------------------------------------------------
	  print(sep)
	  print("Simplest Table: {1}")
	  print(sep)
	  
	  t = {1}
	  printf("Lua:  ")
	  donk(t)
	  -- s = fleece.size(t)
	  -- print("Size: " .. s)
	  s = fleece.json(t, random_esc_flag())
	  print("JSON: " .. s)
	  
	  -----------------------------------------------------------------------
	  
	  print(sep)
	  print("Simple Table: {['foo']='bar',11,22,33,{'a','b'}}")
	  print(sep)
	  
	  t = {['foo']='bar',11,22,33,{'a','b'}}
	  printf("Lua:  ")
	  donk(t)
	  -- s = fleece.size(t)
	  -- print("Size: " .. s)
	  s = fleece.json(t, random_esc_flag())
	  print("JSON: " .. s)
	  
	  -----------------------------------------------------------------------
	  
	  print(sep)
	  print("Random Table of 10 elements, possibly nested")
	  print(sep)
	  
	  t = rndtbl(10)
	  printf("Lua:  ")
	  donk(t)
	  -- s = fleece.size(t)
	  -- print("Size: " .. s)
	  s = fleece.json(t, random_esc_flag())
	  print("JSON: " .. s)
	  
	  -----------------------------------------------------------------------
	  
	  print(sep)
	  print("Random Table of 50 elements, most likely nested")
	  print(sep)
	  
	  t = rndtbl(50)
	  printf("Lua:  ")
	  donk(t)
	  -- s = fleece.size(t)
	  -- print("Size: " .. s)
	  print("................................................................")
	  s = fleece.json(t, random_esc_flag())
	  print("JSON: " .. s)
	  
	  print(sep)
	  
	  -----------------------------------------------------------------------
	  
	  print(sep)
	  print("Random Table of 20 elements, shifting to associative midway")
	  print(sep)
	  
	  t = rndtbl(20, nil, nil, true)
	  printf("Lua:  ")
	  donk(t)
	  -- s = fleece.size(t)
	  -- print("Size: " .. s)
	  print("................................................................")
	  s = fleece.json(t, random_esc_flag())
	  print("JSON: " .. s)
	  
	  print(sep)
	  
	  print("Note that floating numbers have a different precision. That will be controllable by parameter.")

	
	  print(sep2)
	  print("Loop #" .. loopcount)
	  print(sep2)
	  
end
