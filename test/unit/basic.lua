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
---  Use: make unit-tests                                                  ---
---                                                                         ---
-------------------------------------------------------------------------------


package.cpath="src/?.so"
fleece = require("fleece")

module( "basic", lunit.testcase, package.seeall )

-------------------------------------------------------------------------------
 --- json                                                                  ---
-------------------------------------------------------------------------------

function test_string()
  assert_equal( '"hello"', fleece.json("hello"))
  assert_equal( '""', fleece.json(""))
  assert_equal( '"\""', fleece.json("\""))
end

function test_integer()
  assert_equal( '0', fleece.json(0))
  assert_equal( '1', fleece.json(1))
  assert_equal( '-1', fleece.json(-1))
  assert_equal( '1000', fleece.json(1000))
end

function test_float()
  assert_equal( '0', fleece.json(0.0))
  assert_equal( '1', fleece.json(1.0))
  assert_equal( '-1', fleece.json(-1.0))
  assert_equal( '1000', fleece.json(1000.0))
  assert_equal( '0.1', fleece.json(0.1))
  assert_equal( '1.1', fleece.json(1.1))
  assert_equal( '-1.1', fleece.json(-1.1))
  assert_equal( '1000.1', fleece.json(1000.1))
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

-------------------------------------------------------------------------------
 --- size                                                                  ---
-------------------------------------------------------------------------------

function test_size()
	
  assert_equal( 1, fleece.size( 1 ))
  assert_equal( 3, fleece.size( "1" ))
  assert_equal( 3, fleece.size( {1} ))

  assert_equal( string.len('0'), fleece.size(0.0))
  assert_equal( string.len('1'), fleece.size(1.0))
  assert_equal( string.len('-1'), fleece.size(-1.0))
  assert_equal( string.len('1000'), fleece.size(1000.0))
--  assert_equal( string.len('0.1'), fleece.size(0.1)) -- TODO: WRONG!
  assert_equal( string.len('1.1'), fleece.size(1.1)) 
  assert_equal( string.len('-1.1'), fleece.size(-1.1))
  assert_equal( string.len('1000.1'), fleece.size(1000.1))


end
