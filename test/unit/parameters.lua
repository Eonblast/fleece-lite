-------------------------------------------------------------------------------
--- Package     : Fleece - fast Lua to JSON module                          ---
--- File        : test/unit/parameters.lua                                  ---
--- Description : Fleece unit tests: focus on parameters                    ---
--- Version     : 0.3.0 / alpha                                             ---
--- Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     : 08 Mar 2011                                               ---
--- Changed     : 08 Mar 2011                                               ---
-------------------------------------------------------------------------------
---                                                                         ---
---  Unit tests for Fleece, with a focus on parameters to function calls.   ---
---                                                                         ---
---  Use: make unit-tests                                                   ---
---                                                                         ---
-------------------------------------------------------------------------------


package.cpath="src/?.so"
fleece = require("fleece")

module( "parameters", lunit.testcase, package.seeall )

-------------------------------------------------------------------------------

-- used for finding bugs
function disect(x)

    print("disecting " .. x)
    len = x:len()
    c = 1
    while(c <= len) do 
        print(c .. ": " .. x:sub(c,c) .. " (" .. string.byte(x:sub(c,c)) .. ")")
        c = c + 1 
    end

end

function test_single_and_none()

		--/* warm up: hello, empty string, empty table */
		assert_equal( '"hello"', fleece.json("hello"))
		assert_equal( '""', fleece.json(""))
		assert_equal( '[]', fleece.json({}))
		
		--/* main escapers, " and \ */
		assert_equal( '"""', fleece.json("\""))
		assert_equal( '"\\"', fleece.json("\\"))

		assert_equal( '"""', fleece.json("\"", "E0"))
		assert_equal( '"\\"', fleece.json("\\", "E0"))

		assert_equal( '"*"', fleece.json("\"", "E1"))
		assert_equal( '"*"', fleece.json("\\", "E1"))

		assert_equal( '"\\\""', fleece.json("\"", "E2"))
		assert_equal( '"\\\\"', fleece.json("\\", "E2"))

		assert_equal( '"\\\""', fleece.json("\"", "E3"))
		assert_equal( '"\\\\"', fleece.json("\\", "E3"))

		assert_equal( '"\\\""', fleece.json("\"", "E"))
		assert_equal( '"\\\\"', fleece.json("\\", "E"))
end

function test_multiple_escape_strings()

		assert_equal( '"\\\\\\\""', fleece.json("\\\"", "E"))
		assert_equal( '"\'\\\\\'"', fleece.json("'\\'", "E"))
		
		assert_equal( '"\\b"', fleece.json("\b", "E"))
		assert_equal( '"\\t\\t\\t  \\t"', fleece.json("\t\t\t  \t", "E"))
		assert_equal( '"\\r\\n"', fleece.json("\r\n", "E"))
		assert_equal( '"Hello!\\r\\nWorld!"', fleece.json("Hello!\r\nWorld!", "E"))
		
		assert_equal( '"\\u0001\\u0001\\u0001"',
			fleece.json(string.char(1)..string.char(1)..string.char(1), "E"))
-- depends on FLEECE_ZERO_AS_END
--    assert_equal( '"\\u0000x\\u0000"', 
--			fleece.json(string.char(0).."x"..string.char(0), "E"))
--		assert_equal( '"\\u0003\\u0000\\u0004"', 
--			fleece.json(string.char(3)..string.char(0)..string.char(4), "E"))
end

