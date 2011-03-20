-------------------------------------------------------------------------------
--- Package     : Fleece - fast Lua to JSON module                          ---
--- File        : test/unit/escapes.lua                                     ---
--- Description : Fleece unit tests: escape codes                           ---
--- Version     : 0.3.1 / alpha                                             ---
--- Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     : 08 Mar 2011                                               ---
--- Changed     : 18 Mar 2011                                               ---
-------------------------------------------------------------------------------
---                                                                         ---
---  Unit tests for Fleece.
---                                                                         ---
---  Use: make unit-tests on escape codes                                   ---
---                                                                         ---
-------------------------------------------------------------------------------

-- This must be set according to the #define in src/fleece-config.h
-- As a rule, FLEECE_ZERO_AS_END gives up any string contents after a 0,
-- which is legal in Lua, where 0 is allowed as a char value in a string.
FLEECE_ZERO_AS_END = true

package.cpath="src/?.so"
fleece = require("fleece")

module( "escapes", lunit.testcase, package.seeall )

-- ##
-- ## Note that Lua DISCARDS single \ that seem to have no reason, like \h or \/
-- ##

-------------------------------------------------------------------------------
 --- json                                                                  ---
-------------------------------------------------------------------------------

function test_single()

		--/* warm up: hello, empty string, empty table */
		assert_equal( '"hello"', fleece.json("hello"))
		assert_equal( '""', fleece.json(""))
		assert_equal( '[]', fleece.json({}))
		
		--/* main escapers, " and \ note that Lua escapes strings by \ */
		assert_equal( '"\""', fleece.json("\""))
		assert_equal( '"\\"', fleece.json("\\"))
		assert_equal( '"/"', fleece.json("/"))

		assert_equal( '"\""', fleece.json("\"", "E0"))
		assert_equal( '"\\"', fleece.json("\\", "E0"))
		assert_equal( '"/"',  fleece.json("/",  "E0"))

		assert_equal( '"*"', fleece.json("\"", "E1"))
		assert_equal( '"*"', fleece.json("\\", "E1"))
		assert_equal( '"/"', fleece.json("/",  "E1"))

		assert_equal( '"\\\""', fleece.json("\"", "E2"))
		assert_equal( '"\\\\"', fleece.json("\\", "E2"))
		assert_equal( '"/"',    fleece.json("/",  "E2"))

		assert_equal( '"\\\""', fleece.json("\"", "E3"))
		assert_equal( '"\\\\"', fleece.json("\\", "E3"))
		assert_equal( '"\\/"',  fleece.json("/",  "E3"))

		assert_equal( '"\\\""', fleece.json("\"", "E4"))
		assert_equal( '"\\\\"', fleece.json("\\", "E4"))
		assert_equal( '"\\/"',  fleece.json("/",  "E4"))

		assert_equal( '"\\\""', fleece.json("\"", "E"))
		assert_equal( '"\\\\"', fleece.json("\\", "E"))
		assert_equal( '"\\/"',  fleece.json("/",  "E"))
end

function test_single_escape_chars_E0()

		--/* main escapers, " and \ */
		assert_equal( '"""', fleece.json("\"", "E0"))                    -- not escaped, literal """
		assert_equal( '"\\"', fleece.json("\\", "E0"))                   -- not escaped, literal "\"
		assert_equal( '"/"', fleece.json("/", "E0"))                     -- not escaped, literal "/"
		
		--/* named ctrl chars: 8,9,10,12,13: 	\b,\t,\n,\f,\r */
		assert_equal( '"\b"', fleece.json("\b", "E0"))					              -- not escaped 
		assert_equal( '"\t"', fleece.json("\t", "E0"))                                -- not escaped
		assert_equal( '"\n"', fleece.json("\n", "E0"))                                -- not escaped
		assert_equal( '"\f"', fleece.json("\f", "E0"))                                -- not escaped
		assert_equal( '"\r"', fleece.json("\r", "E0"))                                -- not escaped
		assert_equal( '"\b"', fleece.json(string.char( 8), "E0"))                     -- not escaped
		assert_equal( '"\t"', fleece.json(string.char( 9), "E0"))                     -- not escaped
		assert_equal( '"\n"', fleece.json(string.char(10), "E0"))                     -- not escaped
		assert_equal( '"\f"', fleece.json(string.char(12), "E0"))                     -- not escaped
		assert_equal( '"\r"', fleece.json(string.char(13), "E0"))                     -- not escaped
		
		--/* all others < 32 and 127 in \uNNNN format */
		assert_equal( '"'..string.char( 1)..'"', fleece.json(string.char( 1), "E0"))  -- not escaped
		assert_equal( '"'..string.char( 2)..'"', fleece.json(string.char( 2), "E0"))  -- not escaped
		assert_equal( '"'..string.char( 3)..'"', fleece.json(string.char( 3), "E0"))  -- not escaped
		assert_equal( '"'..string.char( 4)..'"', fleece.json(string.char( 4), "E0"))  -- not escaped
		assert_equal( '"'..string.char( 5)..'"', fleece.json(string.char( 5), "E0"))  -- not escaped
		assert_equal( '"'..string.char( 6)..'"', fleece.json(string.char( 6), "E0"))  -- not escaped
		assert_equal( '"'..string.char( 7)..'"', fleece.json(string.char( 7), "E0"))  -- not escaped
		--------------------------------------------------------
		assert_equal( '"'..string.char(11)..'"', fleece.json(string.char(11), "E0"))  -- not escaped
		--------------------------------------------------------
		assert_equal( '"'..string.char(14)..'"', fleece.json(string.char(14), "E0"))  -- not escaped
		assert_equal( '"'..string.char(15)..'"', fleece.json(string.char(15), "E0"))  -- not escaped
		assert_equal( '"'..string.char(16)..'"', fleece.json(string.char(16), "E0"))  -- not escaped
		assert_equal( '"'..string.char(17)..'"', fleece.json(string.char(17), "E0"))  -- not escaped
		assert_equal( '"'..string.char(18)..'"', fleece.json(string.char(18), "E0"))  -- not escaped
		assert_equal( '"'..string.char(19)..'"', fleece.json(string.char(19), "E0"))  -- not escaped
		assert_equal( '"'..string.char(20)..'"', fleece.json(string.char(20), "E0"))  -- not escaped
		assert_equal( '"'..string.char(21)..'"', fleece.json(string.char(21), "E0"))  -- not escaped
		assert_equal( '"'..string.char(22)..'"', fleece.json(string.char(22), "E0"))  -- not escaped
		assert_equal( '"'..string.char(23)..'"', fleece.json(string.char(23), "E0"))  -- not escaped
		assert_equal( '"'..string.char(24)..'"', fleece.json(string.char(24), "E0"))  -- not escaped
		assert_equal( '"'..string.char(25)..'"', fleece.json(string.char(25), "E0"))  -- not escaped
		assert_equal( '"'..string.char(26)..'"', fleece.json(string.char(26), "E0"))  -- not escaped
		assert_equal( '"'..string.char(27)..'"', fleece.json(string.char(27), "E0"))  -- not escaped
		assert_equal( '"'..string.char(28)..'"', fleece.json(string.char(28), "E0"))  -- not escaped
		assert_equal( '"'..string.char(29)..'"', fleece.json(string.char(29), "E0"))  -- not escaped
		assert_equal( '"'..string.char(30)..'"', fleece.json(string.char(30), "E0"))  -- not escaped
		assert_equal( '"'..string.char(31)..'"', fleece.json(string.char(31), "E0"))  -- not escaped
		--------------------------------------------------------
		--/* special last: 0. Lua accepts it as part of the string */
-- TODO: is there a problem with string comparison in Lua when a 0 is part of the string?
-- 		assert_equal( '"'..string.char( 0)..'"', fleece.json(string.char( 0), "E0"))  -- not escaped
		
end

function test_single_escape_chars_E1()

		--/* main escapers, " and \ */
		assert_equal( '"*"', fleece.json("\"", "E1"))                    			  -- purged
		assert_equal( '"*"', fleece.json("\\", "E1"))                                 -- purged
		assert_equal( '"/"', fleece.json("/", "E1"))                                  -- not escaped
		
		--/* named ctrl chars: 8,9,10,12,13: 	\b,\t,\n,\f,\r */
		assert_equal( '"\b"', fleece.json("\b", "E1"))					              -- not escaped 
		assert_equal( '"\t"', fleece.json("\t", "E1"))                                -- not escaped
		assert_equal( '"\n"', fleece.json("\n", "E1"))                                -- not escaped
		assert_equal( '"\f"', fleece.json("\f", "E1"))                                -- not escaped
		assert_equal( '"\r"', fleece.json("\r", "E1"))                                -- not escaped
		assert_equal( '"\b"', fleece.json(string.char( 8), "E1"))                     -- not escaped
		assert_equal( '"\t"', fleece.json(string.char( 9), "E1"))                     -- not escaped
		assert_equal( '"\n"', fleece.json(string.char(10), "E1"))                     -- not escaped
		assert_equal( '"\f"', fleece.json(string.char(12), "E1"))                     -- not escaped
		assert_equal( '"\r"', fleece.json(string.char(13), "E1"))                     -- not escaped
		
		--/* all others < 32 and 127 in \uNNNN format */
		assert_equal( '"'..string.char( 1)..'"', fleece.json(string.char( 1), "E1"))  -- not escaped
		assert_equal( '"'..string.char( 2)..'"', fleece.json(string.char( 2), "E1"))  -- not escaped
		assert_equal( '"'..string.char( 3)..'"', fleece.json(string.char( 3), "E1"))  -- not escaped
		assert_equal( '"'..string.char( 4)..'"', fleece.json(string.char( 4), "E1"))  -- not escaped
		assert_equal( '"'..string.char( 5)..'"', fleece.json(string.char( 5), "E1"))  -- not escaped
		assert_equal( '"'..string.char( 6)..'"', fleece.json(string.char( 6), "E1"))  -- not escaped
		assert_equal( '"'..string.char( 7)..'"', fleece.json(string.char( 7), "E1"))  -- not escaped
		--------------------------------------------------------
		assert_equal( '"'..string.char(11)..'"', fleece.json(string.char(11), "E1"))  -- not escaped
		--------------------------------------------------------
		assert_equal( '"'..string.char(14)..'"', fleece.json(string.char(14), "E1"))  -- not escaped
		assert_equal( '"'..string.char(15)..'"', fleece.json(string.char(15), "E1"))  -- not escaped
		assert_equal( '"'..string.char(16)..'"', fleece.json(string.char(16), "E1"))  -- not escaped
		assert_equal( '"'..string.char(17)..'"', fleece.json(string.char(17), "E1"))  -- not escaped
		assert_equal( '"'..string.char(18)..'"', fleece.json(string.char(18), "E1"))  -- not escaped
		assert_equal( '"'..string.char(19)..'"', fleece.json(string.char(19), "E1"))  -- not escaped
		assert_equal( '"'..string.char(20)..'"', fleece.json(string.char(20), "E1"))  -- not escaped
		assert_equal( '"'..string.char(21)..'"', fleece.json(string.char(21), "E1"))  -- not escaped
		assert_equal( '"'..string.char(22)..'"', fleece.json(string.char(22), "E1"))  -- not escaped
		assert_equal( '"'..string.char(23)..'"', fleece.json(string.char(23), "E1"))  -- not escaped
		assert_equal( '"'..string.char(24)..'"', fleece.json(string.char(24), "E1"))  -- not escaped
		assert_equal( '"'..string.char(25)..'"', fleece.json(string.char(25), "E1"))  -- not escaped
		assert_equal( '"'..string.char(26)..'"', fleece.json(string.char(26), "E1"))  -- not escaped
		assert_equal( '"'..string.char(27)..'"', fleece.json(string.char(27), "E1"))  -- not escaped
		assert_equal( '"'..string.char(28)..'"', fleece.json(string.char(28), "E1"))  -- not escaped
		assert_equal( '"'..string.char(29)..'"', fleece.json(string.char(29), "E1"))  -- not escaped
		assert_equal( '"'..string.char(30)..'"', fleece.json(string.char(30), "E1"))  -- not escaped
		assert_equal( '"'..string.char(31)..'"', fleece.json(string.char(31), "E1"))  -- not escaped
		--------------------------------------------------------
		--/* special last: 0. Lua accepts it as part of the string */
-- TODO: is there a problem with string comparison in Lua when a 0 is part of the string?
-- clarify		assert_equal( '"'..string.char( 0)..'"', fleece.json(string.char( 0), "E1"))  -- not escaped
		
end

function test_single_escape_chars_E2()

		--/* main escapers, " and \ */
		assert_equal( '"\\""', fleece.json("\"", "E2"))                    			  -- escaped
		assert_equal( '"\\\\"', fleece.json("\\", "E2"))                              -- escaped
		assert_equal( '"/"', fleece.json("/", "E2"))                                  -- not escaped
		
		--/* named ctrl chars: 8,9,10,12,13: 	\b,\t,\n,\f,\r */
		assert_equal( '"\b"', fleece.json("\b", "E2"))					              -- not escaped 
		assert_equal( '"\t"', fleece.json("\t", "E2"))                                -- not escaped
		assert_equal( '"\n"', fleece.json("\n", "E2"))                                -- not escaped
		assert_equal( '"\f"', fleece.json("\f", "E2"))                                -- not escaped
		assert_equal( '"\r"', fleece.json("\r", "E2"))                                -- not escaped
		assert_equal( '"\b"', fleece.json(string.char( 8), "E2"))                     -- not escaped
		assert_equal( '"\t"', fleece.json(string.char( 9), "E2"))                     -- not escaped
		assert_equal( '"\n"', fleece.json(string.char(10), "E2"))                     -- not escaped
		assert_equal( '"\f"', fleece.json(string.char(12), "E2"))                     -- not escaped
		assert_equal( '"\r"', fleece.json(string.char(13), "E2"))                     -- not escaped
		
		--/* all others < 32 and 127 in \uNNNN format */
		assert_equal( '"'..string.char( 1)..'"', fleece.json(string.char( 1), "E2"))  -- not escaped
		assert_equal( '"'..string.char( 2)..'"', fleece.json(string.char( 2), "E2"))  -- not escaped
		assert_equal( '"'..string.char( 3)..'"', fleece.json(string.char( 3), "E2"))  -- not escaped
		assert_equal( '"'..string.char( 4)..'"', fleece.json(string.char( 4), "E2"))  -- not escaped
		assert_equal( '"'..string.char( 5)..'"', fleece.json(string.char( 5), "E2"))  -- not escaped
		assert_equal( '"'..string.char( 6)..'"', fleece.json(string.char( 6), "E2"))  -- not escaped
		assert_equal( '"'..string.char( 7)..'"', fleece.json(string.char( 7), "E2"))  -- not escaped
		--------------------------------------------------------
		assert_equal( '"'..string.char(11)..'"', fleece.json(string.char(11), "E2"))  -- not escaped
		--------------------------------------------------------
		assert_equal( '"'..string.char(14)..'"', fleece.json(string.char(14), "E2"))  -- not escaped
		assert_equal( '"'..string.char(15)..'"', fleece.json(string.char(15), "E2"))  -- not escaped
		assert_equal( '"'..string.char(16)..'"', fleece.json(string.char(16), "E2"))  -- not escaped
		assert_equal( '"'..string.char(17)..'"', fleece.json(string.char(17), "E2"))  -- not escaped
		assert_equal( '"'..string.char(18)..'"', fleece.json(string.char(18), "E2"))  -- not escaped
		assert_equal( '"'..string.char(19)..'"', fleece.json(string.char(19), "E2"))  -- not escaped
		assert_equal( '"'..string.char(20)..'"', fleece.json(string.char(20), "E2"))  -- not escaped
		assert_equal( '"'..string.char(21)..'"', fleece.json(string.char(21), "E2"))  -- not escaped
		assert_equal( '"'..string.char(22)..'"', fleece.json(string.char(22), "E2"))  -- not escaped
		assert_equal( '"'..string.char(23)..'"', fleece.json(string.char(23), "E2"))  -- not escaped
		assert_equal( '"'..string.char(24)..'"', fleece.json(string.char(24), "E2"))  -- not escaped
		assert_equal( '"'..string.char(25)..'"', fleece.json(string.char(25), "E2"))  -- not escaped
		assert_equal( '"'..string.char(26)..'"', fleece.json(string.char(26), "E2"))  -- not escaped
		assert_equal( '"'..string.char(27)..'"', fleece.json(string.char(27), "E2"))  -- not escaped
		assert_equal( '"'..string.char(28)..'"', fleece.json(string.char(28), "E2"))  -- not escaped
		assert_equal( '"'..string.char(29)..'"', fleece.json(string.char(29), "E2"))  -- not escaped
		assert_equal( '"'..string.char(30)..'"', fleece.json(string.char(30), "E2"))  -- not escaped
		assert_equal( '"'..string.char(31)..'"', fleece.json(string.char(31), "E2"))  -- not escaped
		--------------------------------------------------------
		--/* special last: 0. Lua accepts it as part of the string */
-- TODO: is there a problem with string comparison in Lua when a 0 is part of the string?
-- clarify		assert_equal( '"'..string.char( 0)..'"', fleece.json(string.char( 0), "E2"))  -- not escaped
		
end

function test_single_escape_chars_E3()

		--/* main escapers, " and \ */
		assert_equal( '"\\""', fleece.json("\"", "E3"))                    			  -- escaped
		assert_equal( '"\\\\"', fleece.json("\\", "E3"))                              -- escaped
		assert_equal( '"\\/"', fleece.json("/", "E3"))                                -- escaped
		
		--/* named ctrl chars: 8,9,10,12,13: 	\b,\t,\n,\f,\r */
		assert_equal( '"\b"', fleece.json("\b", "E3"))					              -- not escaped 
		assert_equal( '"\t"', fleece.json("\t", "E3"))                                -- not escaped
		assert_equal( '"\n"', fleece.json("\n", "E3"))                                -- not escaped
		assert_equal( '"\f"', fleece.json("\f", "E3"))                                -- not escaped
		assert_equal( '"\r"', fleece.json("\r", "E3"))                                -- not escaped
		assert_equal( '"\b"', fleece.json(string.char( 8), "E3"))                     -- not escaped
		assert_equal( '"\t"', fleece.json(string.char( 9), "E3"))                     -- not escaped
		assert_equal( '"\n"', fleece.json(string.char(10), "E3"))                     -- not escaped
		assert_equal( '"\f"', fleece.json(string.char(12), "E3"))                     -- not escaped
		assert_equal( '"\r"', fleece.json(string.char(13), "E3"))                     -- not escaped
		
		--/* all others < 32 and 127 in \uNNNN format */
		assert_equal( '"'..string.char( 1)..'"', fleece.json(string.char( 1), "E3"))  -- not escaped
		assert_equal( '"'..string.char( 2)..'"', fleece.json(string.char( 2), "E3"))  -- not escaped
		assert_equal( '"'..string.char( 3)..'"', fleece.json(string.char( 3), "E3"))  -- not escaped
		assert_equal( '"'..string.char( 4)..'"', fleece.json(string.char( 4), "E3"))  -- not escaped
		assert_equal( '"'..string.char( 5)..'"', fleece.json(string.char( 5), "E3"))  -- not escaped
		assert_equal( '"'..string.char( 6)..'"', fleece.json(string.char( 6), "E3"))  -- not escaped
		assert_equal( '"'..string.char( 7)..'"', fleece.json(string.char( 7), "E3"))  -- not escaped
		--------------------------------------------------------
		assert_equal( '"'..string.char(11)..'"', fleece.json(string.char(11), "E3"))  -- not escaped
		--------------------------------------------------------
		assert_equal( '"'..string.char(14)..'"', fleece.json(string.char(14), "E3"))  -- not escaped
		assert_equal( '"'..string.char(15)..'"', fleece.json(string.char(15), "E3"))  -- not escaped
		assert_equal( '"'..string.char(16)..'"', fleece.json(string.char(16), "E3"))  -- not escaped
		assert_equal( '"'..string.char(17)..'"', fleece.json(string.char(17), "E3"))  -- not escaped
		assert_equal( '"'..string.char(18)..'"', fleece.json(string.char(18), "E3"))  -- not escaped
		assert_equal( '"'..string.char(19)..'"', fleece.json(string.char(19), "E3"))  -- not escaped
		assert_equal( '"'..string.char(20)..'"', fleece.json(string.char(20), "E3"))  -- not escaped
		assert_equal( '"'..string.char(21)..'"', fleece.json(string.char(21), "E3"))  -- not escaped
		assert_equal( '"'..string.char(22)..'"', fleece.json(string.char(22), "E3"))  -- not escaped
		assert_equal( '"'..string.char(23)..'"', fleece.json(string.char(23), "E3"))  -- not escaped
		assert_equal( '"'..string.char(24)..'"', fleece.json(string.char(24), "E3"))  -- not escaped
		assert_equal( '"'..string.char(25)..'"', fleece.json(string.char(25), "E3"))  -- not escaped
		assert_equal( '"'..string.char(26)..'"', fleece.json(string.char(26), "E3"))  -- not escaped
		assert_equal( '"'..string.char(27)..'"', fleece.json(string.char(27), "E3"))  -- not escaped
		assert_equal( '"'..string.char(28)..'"', fleece.json(string.char(28), "E3"))  -- not escaped
		assert_equal( '"'..string.char(29)..'"', fleece.json(string.char(29), "E3"))  -- not escaped
		assert_equal( '"'..string.char(30)..'"', fleece.json(string.char(30), "E3"))  -- not escaped
		assert_equal( '"'..string.char(31)..'"', fleece.json(string.char(31), "E3"))  -- not escaped
		--------------------------------------------------------
		--/* special last: 0. Lua accepts it as part of the string */
-- TODO: is there a problem with string comparison in Lua when a 0 is part of the string?
-- clarify		assert_equal( '"'..string.char( 0)..'"', fleece.json(string.char( 0), "E3"))  -- not escaped
		
end

function test_single_escape_chars_E4()

		--/* main escapers, " and \ */
		assert_equal( '"\\""', fleece.json("\"", "E4"))                    			      -- escaped
		assert_equal( '"\\\\"', fleece.json("\\", "E4"))                                  -- escaped
		assert_equal( '"\\/"', fleece.json("/", "E4"))                                    -- escaped
		
		--/* named ctrl chars: 8,9,10,12,13: 	\b,\t,\n,\f,\r */
		assert_equal( '"\\b"', fleece.json("\b", "E4"))					                  -- escaped 
		assert_equal( '"\\t"', fleece.json("\t", "E4"))                                   -- escaped
		assert_equal( '"\\n"', fleece.json("\n", "E4"))                                   -- escaped
		assert_equal( '"\\f"', fleece.json("\f", "E4"))                                   -- escaped
		assert_equal( '"\\r"', fleece.json("\r", "E4"))                                   -- escaped
		assert_equal( '"\\b"', fleece.json(string.char( 8), "E4"))                        -- escaped
		assert_equal( '"\\t"', fleece.json(string.char( 9), "E4"))                        -- escaped
		assert_equal( '"\\n"', fleece.json(string.char(10), "E4"))                        -- escaped
		assert_equal( '"\\f"', fleece.json(string.char(12), "E4"))                        -- escaped
		assert_equal( '"\\r"', fleece.json(string.char(13), "E4"))                        -- escaped
		
		--/* all others < 32 and 127 in \uNNNN format */
		assert_equal( '"\\u0001"', fleece.json(string.char( 1), "E4"))                    -- escaped
		assert_equal( '"\\u0002"', fleece.json(string.char( 2), "E4"))                    -- escaped
		assert_equal( '"\\u0003"', fleece.json(string.char( 3), "E4"))                    -- escaped
		assert_equal( '"\\u0004"', fleece.json(string.char( 4), "E4"))                    -- escaped
		assert_equal( '"\\u0005"', fleece.json(string.char( 5), "E4"))                    -- escaped
		assert_equal( '"\\u0006"', fleece.json(string.char( 6), "E4"))                    -- escaped
		assert_equal( '"\\u0007"', fleece.json(string.char( 7), "E4"))                    -- escaped
		--------------------------------------------------------
		assert_equal( '"\\u000b"', fleece.json(string.char(11), "E4"))                    -- escaped
		--------------------------------------------------------
		assert_equal( '"\\u000e"', fleece.json(string.char(14), "E4"))                    -- escaped
		assert_equal( '"\\u000f"', fleece.json(string.char(15), "E4"))                    -- escaped
		assert_equal( '"\\u0010"', fleece.json(string.char(16), "E4"))                    -- escaped
		assert_equal( '"\\u0011"', fleece.json(string.char(17), "E4"))                    -- escaped
		assert_equal( '"\\u0012"', fleece.json(string.char(18), "E4"))                    -- escaped
		assert_equal( '"\\u0013"', fleece.json(string.char(19), "E4"))                    -- escaped
		assert_equal( '"\\u0014"', fleece.json(string.char(20), "E4"))                    -- escaped
		assert_equal( '"\\u0015"', fleece.json(string.char(21), "E4"))                    -- escaped
		assert_equal( '"\\u0016"', fleece.json(string.char(22), "E4"))                    -- escaped
		assert_equal( '"\\u0017"', fleece.json(string.char(23), "E4"))                    -- escaped
		assert_equal( '"\\u0018"', fleece.json(string.char(24), "E4"))                    -- escaped
		assert_equal( '"\\u0019"', fleece.json(string.char(25), "E4"))                    -- escaped
		assert_equal( '"\\u001a"', fleece.json(string.char(26), "E4"))                    -- escaped
		assert_equal( '"\\u001b"', fleece.json(string.char(27), "E4"))                    -- escaped
		assert_equal( '"\\u001c"', fleece.json(string.char(28), "E4"))                    -- escaped
		assert_equal( '"\\u001d"', fleece.json(string.char(29), "E4"))                    -- escaped
		assert_equal( '"\\u001e"', fleece.json(string.char(30), "E4"))                    -- escaped
		assert_equal( '"\\u001f"', fleece.json(string.char(31), "E4"))                    -- escaped
		--------------------------------------------------------
		--/* special last: 0. Lua accepts it as part of the string */
        if not FLEECE_ZERO_AS_END then
        	assert_equal( '"\\u0000"', fleece.json(string.char( 0), "E4"))                    -- escaped
		end
end

function test_multiple_escape_strings()

		assert_equal( '"\\\\\\\""', fleece.json("\\\"", "E")) -- \" -> \\\" (sic)
		assert_equal( '"\'\\\\\'"', fleece.json("'\\'", "E")) -- '\' -> '\\'(sic) 
		
		assert_equal( '"\\b"', fleece.json("\b", "E"))
		assert_equal( '"\\t\\t\\t  \\t"', fleece.json("\t\t\t  \t", "E"))
		assert_equal( '"\\r\\n"', fleece.json("\r\n", "E"))
		assert_equal( '"Hello!\\r\\nWorld!"', fleece.json("Hello!\r\nWorld!", "E"))
		
		assert_equal( '"\\u0001\\u0001\\u0001"',
			fleece.json(string.char(1)..string.char(1)..string.char(1), "E"))
        if not FLEECE_ZERO_AS_END then
           assert_equal( '"\\u0000x\\u0000"', 
               fleece.json(string.char(0).."x"..string.char(0), "E"))
           assert_equal( '"\\u0003\\u0000\\u0004"', 
               fleece.json(string.char(3)..string.char(0)..string.char(4), "E"))
        end
end

function test_multiple_mixed_escape_strings_E()

		assert_equal( '"hey!\\\\\\""', fleece.json("\hey!\\\"", "E")) -- hey!\" (sic)                  -- "h  sic!?!
		assert_equal( '"ah!hey!\\\\\\""', fleece.json("ah!\hey!\\\"", "E")) -- ah!hey!\" (sic)
		assert_equal( '"hey!\\\\\\"rrr\\""', fleece.json("\hey!\\\"rrr\"", "E"))       -- !h  sic!?!
		assert_equal( '"hey!\\\\b\\\\c\\""', fleece.json("\hey!\\b\\\c\"", "E"))       -- !h  sic!?!
		assert_equal( '"\'\\\\\'"', fleece.json("'\\'", "E"))
		assert_equal( '"\\u0001\\u0001\\u0001"',
			fleece.json(string.char(1)..string.char(1)..string.char(1), "E"))
        if not FLEECE_ZERO_AS_END then
            assert_equal( '"\\u0000x\\u0000"', 
                fleece.json(string.char(0).."x"..string.char(0), "E"))
            assert_equal( '"\\u0003\\u0000\\u0004"', 
                fleece.json(string.char(3)..string.char(0)..string.char(4), "E"))
        end

end

function test_multiple_mixed_escape_strings_E0()

-- \h?	assert_equal( '"\hey!\\\""', fleece.json("\hey!\\\"", "E0"))
		assert_equal( '"ah!\hey!\\\""', fleece.json("ah!\hey!\\\"", "E0"))
		assert_equal( '"\hey!\\\"rrr\""', fleece.json("\hey!\\\"rrr\"", "E0"))
		assert_equal( '"\hey!/\"rrr\"/"', fleece.json("\hey!/\"rrr\"/", "E0"))
		assert_equal( '"\hey!\\b\\\c\""', fleece.json("\hey!\\b\\\c\"", "E0"))
		assert_equal( '"\'\\\'"', fleece.json("'\\'", "E0"))
		assert_equal( '"'..string.char(1)..string.char(1)..string.char(1)..'"',
			fleece.json(string.char(1)..string.char(1)..string.char(1), "E0"))

        if not FLEECE_ZERO_AS_END then
-- TODO: is there a problem with string comparison in Lua when a 0 is part of the string?
-- 0		assert_equal( '"'..string.char(0).."x"..string.char(0)..'"', 
-- 0			fleece.json(string.char(0).."x"..string.char(0), "E0"))
-- 0		assert_equal( '"'..string.char(3)..string.char(0)..string.char(4)..'"', 
-- 0			fleece.json(string.char(3)..string.char(0)..string.char(4), "E0"))
        end
end

function test_multiple_mixed_escape_strings_E1()
                                                                             -- TODO: what's with \h?
		assert_equal( '"hey!**"', fleece.json("\hey!\\\"", "E1"))                       -- "h  sic!?!
		assert_equal( '"ah!hey!**"', fleece.json("ah!\hey!\\\"", "E1"))                 -- "h  sic!?!
		assert_equal( '"hey!**rrr*"', fleece.json("\hey!\\\"rrr\"", "E1"))              -- "h  sic!?!
		assert_equal( '"hey!/*rrr*/"', fleece.json("\hey!/\"rrr\"/", "E1"))             -- "h  sic!?!
		assert_equal( '"hey!*b*c*"', fleece.json("\hey!\\b\\\c\"", "E1"))               -- "h  sic!?!
		assert_equal( '"\'*\'"', fleece.json("'\\'", "E1"))
		assert_equal( '"'..string.char(1)..string.char(1)..string.char(1)..'"',
			fleece.json(string.char(1)..string.char(1)..string.char(1), "E1"))

        if not FLEECE_ZERO_AS_END then
-- TODO: is there a problem with string comparison in Lua when a 0 is part of the string?
-- 0		assert_equal( '"'..string.char(0).."x"..string.char(0)..'"', 
-- 0			fleece.json(string.char(0).."x"..string.char(0), "E1"))
-- 0		assert_equal( '"'..string.char(3)..string.char(0)..string.char(4)..'"', 
-- 0			fleece.json(string.char(3)..string.char(0)..string.char(4), "E1"))
        end
end

function test_multiple_mixed_escape_strings_E2()
                                                                             -- TODO: what's with \h?
		assert_equal( '"hey!\\\\\\""', fleece.json("\hey!\\\"", "E2"))                  -- "h  sic!?!
		assert_equal( '"ah!hey!\\\\\\""', fleece.json("ah!\hey!\\\"", "E2"))            -- !h  sic!?!
		assert_equal( '"hey!\\\\\\"rrr\\""', fleece.json("\hey!\\\"rrr\"", "E2"))       -- !h  sic!?!
		assert_equal( '"hey!/\\\"rrr\\\"/"', fleece.json("\hey!/\"rrr\"/", "E2"))       -- "h  sic!?!
		assert_equal( '"hey!\\\\b\\\\c\\""', fleece.json("\hey!\\b\\\c\"", "E2"))       -- !h  sic!?!
		assert_equal( '"\'\\\\\'"', fleece.json("'\\'", "E2"))
		assert_equal( '"'..string.char(1)..string.char(1)..string.char(1)..'"',
			fleece.json(string.char(1)..string.char(1)..string.char(1), "E2"))

        if not FLEECE_ZERO_AS_END then
-- TODO: is there a problem with string comparison in Lua when a 0 is part of the string?
-- 0		assert_equal( '"'..string.char(0).."x"..string.char(0)..'"',
-- 0			fleece.json(string.char(0).."x"..string.char(0), "E2"))
-- 0		assert_equal( '"'..string.char(3)..string.char(0)..string.char(4)..'"', 
-- 0			fleece.json(string.char(3)..string.char(0)..string.char(4), "E2"))
        end
end

function test_multiple_mixed_escape_strings_E3()
                                                                             -- TODO: what's with \h?
		assert_equal( '"hey!\\\\\\""', fleece.json("\hey!\\\"", "E3"))                  -- "h  sic!?!
		assert_equal( '"ah!hey!\\\\\\""', fleece.json("ah!\hey!\\\"", "E3"))            -- !h  sic!?!
		assert_equal( '"hey!\\\\\\"rrr\\""', fleece.json("\hey!\\\"rrr\"", "E3"))       -- !h  sic!?!
		assert_equal( '"hey!\\/\\\"rrr\\\"\\/"', fleece.json("\hey!/\"rrr\"/", "E3"))   -- "h  sic!?!
		assert_equal( '"hey!\\\\b\\\\c\\""', fleece.json("\hey!\\b\\\c\"", "E3"))       -- !h  sic!?!
		assert_equal( '"\'\\\\\'"', fleece.json("'\\'", "E3"))
		assert_equal( '"'..string.char(1)..string.char(1)..string.char(1)..'"',
			fleece.json(string.char(1)..string.char(1)..string.char(1), "E3"))

        if not FLEECE_ZERO_AS_END then
-- TODO: is there a problem with string comparison in Lua when a 0 is part of the string?
-- 0		assert_equal( '"'..string.char(0).."x"..string.char(0)..'"',
-- 0			fleece.json(string.char(0).."x"..string.char(0), "E3"))
-- 0		assert_equal( '"'..string.char(3)..string.char(0)..string.char(4)..'"', 
-- 0			fleece.json(string.char(3)..string.char(0)..string.char(4), "E3"))
        end
end

function test_multiple_mixed_escape_strings_E4()
                                                                             -- TODO: what's with \h?
		assert_equal( '"hey!\\\\\\""', fleece.json("\hey!\\\"", "E4"))                  -- "h  sic!?!
		assert_equal( '"ah!hey!\\\\\\""', fleece.json("ah!\hey!\\\"", "E4"))            -- !h  sic!?!
		assert_equal( '"hey!\\\\\\"rrr\\""', fleece.json("\hey!\\\"rrr\"", "E4"))       -- !h  sic!?!
		assert_equal( '"hey!\\\\b\\\\c\\""', fleece.json("\hey!\\b\\\c\"", "E4"))       -- !h  sic!?!
		assert_equal( '"\'\\\\\'"', fleece.json("'\\'", "E4"))
		assert_equal( '"\\u0001\\u0001\\u0001"',
			fleece.json(string.char(1)..string.char(1)..string.char(1), "E4"))
        if not FLEECE_ZERO_AS_END then
            assert_equal( '"\\u0000x\\u0000"', 
               	fleece.json(string.char(0).."x"..string.char(0), "E4"))
            assert_equal( '"\\u0003\\u0000\\u0004"', 
                fleece.json(string.char(3)..string.char(0)..string.char(4), "E4"))
        end
end
