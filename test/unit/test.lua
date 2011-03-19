-------------------------------------------------------------------------------
--- Package     : Fleece - fast Lua to JSON module                          ---
--- File        : test/unit/basics.lua                                      ---
--- Description : Fleece unit tests: basic set of tests                     ---
--- Version     : 0.3.0 / alpha                                             ---
--- Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     : 05 Mar 2011                                               ---
--- Changed     : 05 Mar 2011                                               ---
-------------------------------------------------------------------------------
---                                                                         ---
---  Unit tests for Fleece.
---                                                                         ---
---  Use: lua test/bench-fleece.lua                                         ---
---  Or:  make basic-tests #                                                ---
---  Or:  make unit-tests #                                                 ---
---                                                                         ---
-------------------------------------------------------------------------------

function pre()
	print("Fleece Unit Tets")
	print("=========================================================")
end

package.cpath="src/?.so"
fleece = require("fleece")
-- package.path="etc/lunit/lunit.lua"
-- require("lunit")

module( "test", lunit.testcase, package.seeall )

function test_string()
  assert_equal( '"hello"', fleece.json("hello"))
end

function test_boolean()
  assert_equal( "true", fleece.json(true))
  assert_equal( "false", fleece.json(false))
  assert_not_equal( "true", fleece.json(True))
  assert_not_equal( "false", fleece.json(False))
end

function test_table()
  assert_equal( "[1]", fleece.json( {1} ))
  assert_equal( "[1,2,3]", fleece.json( {1,2,3} ))
  assert_equal( "[]", fleece.json( {} ))
  assert_equal( "[[]]", fleece.json( {{}} ))
end
