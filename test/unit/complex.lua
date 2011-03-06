-------------------------------------------------------------------------------
--- Package     : Fleece - fast Lua to JSON module                          ---
--- File        : test/unit/complex.lua                                     ---
--- Description : Fleece unit tests: more complex tests                     ---
--- Version     : 0.2.4 / alpha                                             ---
--- Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     : 05 Mar 2011                                               ---
--- Changed     : 05 Mar 2011                                               ---
-------------------------------------------------------------------------------
---                                                                         ---
---  Complex unit tests for Fleece that take a little longer                ---
---                                                                         ---
---  Use: in Fleece root directory 'make unit-tests'                        ---
---                                                                         ---
-------------------------------------------------------------------------------


package.cpath="src/?.so"
fleece = require("fleece")

module( "complex", lunit.testcase, package.seeall )

-------------------------------------------------------------------------------
-- utility
-------------------------------------------------------------------------------
local abc = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'}

-- Creation of Random Strings
function randstr(length)
	local r = {}
	local i
	for i = 1, length do
		r[i] = abc[math.random(1, 26)]
	end

	return table.concat(r)
end

-- Creation of Fixed Strings
function repstr(length)

	if length == 0 then return "" end

	local r = {}
	local i
	for i = 1, length do
		r[i] = abc[(i-1) % 26 + 1]
	end

	return table.concat(r)
end

-- Creation of nested tables
function nest(depth, inner)
	if depth == 0 then 
		t = {}; t[1] = inner; return t, "[\""..inner.."\"]" end
	gett, gets = nest(depth-1,inner)
	ret = { gett }
	res = "[" .. gets .. "]"
	return ret, res
end

-------------------------------------------------------------------------------
-- tests
-------------------------------------------------------------------------------


-- strings --------------------------------------------------------------------

function subtest_string(min, max)

	sx = complex.repstr(max)
	for i = min, max do
		s = sx:sub(min,i)
		exp = '"'..s..'"'
		io.write(string.format("test string of length %8d    %.20s..", i, exp))
		io.flush()
		assert_equal( exp, fleece.json(s))
		print(" ok")
	end
end

function ptest_string()

	io.write(" strings ...")

	-- test all strings of length 1 to 1110
	subtest_string(1,1110)
	
	-- test all strings of length 9999 to 10099
	subtest_string(9999,10099)

	-- test all strings of length 10999 to 11099
	subtest_string(10999,11099)

	-- test all strings of length 999999 to 1000003
	subtest_string(999999,1000003)

	-- test 0 length
	subtest_string(0,0)

end


-- table ----------------------------------------------------------------------

function subtest_table(depth, innerlength)

	t,s = nest(depth, repstr(innerlength))
	print("table nesting " .. 
	      string.format("%5d", depth)..
	      " levels deep, with inside string of length " .. 
	      string.format("%5d", innerlength)..
	      "     " .. s:sub(1,20) .. "..")
	assert_equal( s, fleece.json( t ))

end

function test_table()

	io.write(" tables ...")

	-- these numbers are used both for nest depth and the
	-- inner string's length. See output when running
	interesting = { 0,1,2,3,4,5,6,7,8,9,10,11,12,
					97,98,99,100,101,102,
					997,998,999,1000,1001,1002,
					9997,9998,9999,10000,10001,10002,
					10097,10098,10099,10100,10101,10102 }

	for i = 1, #interesting do
		for j = 1, #interesting do
			depth = interesting[i]
			innerlen = interesting[j]

			subtest_table(depth, innerlen)
		end
	end

end


