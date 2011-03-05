-------------------------------------------------------------------------------
--- Package     : Fleece - fast Lua to JSON module                          ---
--- File        : test/unit/basics.lua                                      ---
--- Description : Fleece unit tests: basic set of tests                     ---
--- Version     : 0.2.3 / alpha                                             ---
--- Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     : 05 Mar 2011                                               ---
--- Changed     : 05 Mar 2011                                               ---
-------------------------------------------------------------------------------
---                                                                         ---
---  Unit tests for Fleece.
---                                                                         ---
---  Use: make unit-tests                                                  ---
---                                                                         ---
-------------------------------------------------------------------------------


package.cpath="src/?.so"
fleece = require("fleece")

module( "basic", lunit.testcase, package.seeall )

function test_string()
  assert_equal( '"hello"', fleece.json("hello"))
  assert_equal( '""', fleece.json(""))
  assert_equal( '"\""', fleece.json("\""))
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
