**Eonblast Fleece 0.3.1**

# Fleece Tests, Benchmarks, Comparison

**Use *make* to execute tests and benchmarks. The scripts for it are in the test/ folder**

For a complete list of related make options, see section **Make Options**.

For a list of files in the test directory, see last section, **Files**.

## Tests

For a most simple test, after building, do

    make test
    
You should see this result:

    This tests, wether the Fleece library can be loaded at all:
    Calling fleece.hello() -- should print: Hello LuaJSON World!: Hello LuaJSON World!
    This tests, wether json() responds to a most simple call:
    Calling fleece.json({'ok'})  -- should print ["ok"]: ["ok"]

For a brief but comprehensive test, after building, do

    make check


For more tests, see below, section **Make: Tests**, or run `make` without parameters.

## Benchmarks

To test speed, run the basic benchmark test:

    make bench
    
This will print out something like this:

    Fleece Benchmarks
    =================
    A couple of random tables are created and speed is clocked.
    You should have built fleece first with 'make <PLATFORM>', 
    and now be in the fleece root directory.
    Lua 5.1 official - Fleece 0.3.1
    -------------------------------------------------------------------------
    i=[1..1000] in t[i]=i       for 1000 cycles of fleece.json(t)
    . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    prep table ... action ...         50 nanosec/element =>  20M elements/sec 
    -------------------------------------------------------------------------
    i=[1..1000] in t['x'..i]=i  for 1000 cycles of fleece.json(t)
    . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    prep table ... action ...        100 nanosec/element =>  10M elements/sec 
    -------------------------------------------------------------------------
    i=[1..1000] in t[i]=randstr(i) for 1000 cycles of fleece.json(t)
    . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    prep table ... action ...         90 nanosec/element =>  11M elements/sec 
    -------------------------------------------------------------------------
    i=[1..1000] in t[randstr(i)]=randstr(i) for 1000 cycles of fleece.json(t)
    . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    prep table ... action ...        160 nanosec/element =>   6M elements/sec 
    -------------------------------------------------------------------------
    There are more benchmarks. Try bench2, bench3a, bench7a.
    Run 'make' w/o parameters for details, or see test/README.md.
    Repeat runs will differ. For finer grained figures use bench-10k. 
    Also see test/README.md

For more benchmarks, see below, section **Make: Benchmarks**, or run `make` without parameters.

## Comparison

There are comparisons available for Fleece with:

* **JSON4 0.9.50**: aka JSON4Lua, JSON for Lua, module name 'json' <http://json.luaforge.net/>
* **LuaJSON 1.1**: aka luajsonlib, LuaJSON C Library <http://luaforge.net/projects/luajsonlib/>
* **Lua-Yajl 1.0.9**: with Oct '10  bindings <https://github.com/brimworks/lua-yajl/>

Note that the names of the various Lua-JSON packages can be confusing. They are not unique and therefore 'JSON4' and 'LuaJSON' are chosen here to clarify which packages are meant. They don't always call themselves this way.

JSON4 is native Lua, LuaJSON and Lua-Yajl are C libraries for Lua and thus faster. JSON4 is listed as 'cmjjson' with the comparisons at <http://lua-users.org/wiki/JsonModules>. 

If somebody has the bench code behind the numbers at <http://lua-users.org/wiki/JsonModules>, please let me know.

LuaJSON needed slight modifications for me to compile, see /* hd */ tags int the source. It sometimes crashes for me on one Mac, not sure why. To take the blame from Fleece, try bench3a then. Luajson comes with this package, in etc/luajson and can be made with 'make &lt;PLATFORM>-test'.
 
Installing Lua-Yajl is no real fun and you may want to skip that if your interest in the benchmarks is casual. There is no support for installing lua-yajl. You'll miss out on bench4, 5 and 5c but that is no real loss as Lua-Yajl beforms by and large similar to LuaJSON. Make sure to run 5a once and let me know if you ran into trouble (an eventual out of memory is ok).

To run a basic comparison, run

    make bench2

This will result into a screen like this:

    Fleece Benchmarks vs Json4, a native Lua JSON implementation
    ============================================================
    A couple of random tables are created and speed is clocked.
    You should have built fleece first with 'make <PLATFORM>' 
    and should now be in the fleece root directory.
    Lua 5.1 official - Fleece 0.3.1
    ---------------------------------------------------------------------------------
    1000 elements in t[i]=i                   
    100x json4.encode(t)             3400ns/element      [1,2,3,4,5,6,7,8,9,1.. 
    100x fleece.json(t)               100ns/element 2.9% [1,2,3,4,5,6,7,8,9,1.. 
    ---------------------------------------------------------------------------------
    1000 elements in t['x'..i]=i              
    100x json4.encode(t)             6500ns/element      {"x97":97,"x366":366.. 
    100x fleece.json(t)               100ns/element 1.5% {"x518":518,"x744":7.. 
    ---------------------------------------------------------------------------------
    1000 elements in t[i]=randstr(i)          
    100x json4.encode(t)             3600ns/element      ["kuuxfithomqjnyxqs".. 
    100x fleece.json(t)               100ns/element 2.7% ["aaryqnuvdyw","l",".. 
    ---------------------------------------------------------------------------------
    1000 elements in t[randstr(i)]=randstr(i) 
    100x json4.encode(t)             5400ns/element      {"dsvzrwfogbvhwjqvgi.. 
    100x fleece.json(t)               100ns/element 1.8% {"lqx":"rbknxviclt",.. 
    ---------------------------------------------------------------------------------
    Note that fleece may list associative arrays in different order.
    Also see test/README.md

If you encounter a message, telling you that the sample was too small, use bench2-1k instead.

To compare with the more potent C library LuaJSON, do:

    make <PLATFORM-TEST>
    make bench3
    
Where PLATFORM-TEST is one of

    linux-test linux-64-test macosx-test macosx-old-test

E.g.:

    make linux-64-test
    make bench3

This will print a screen similar to the above, of bench2.

See section after next, **Make: Benchmark Execution** on how to run comparison benchmarks.

# Make Options

To make tests, benchmarks and comparisons, these options are available directly by using make from the root directory. To get the most up to date list of the options below, execute make in the root directory, without any parameters.

### Make: Building LuaJSON for Comparison

LuaJSON and JSON4 are bundled with Fleece, for comparison in the bench marks. JSON4 works out of the box, being native Lua. LuaJSON needs to be built:

	  make linux-test
	  make linux-64-test
	  make macosx-test
	  make macosx-old-test
		these build the LuaJSON C lib, which comes bundled, for comparison
		test by using 'make test-luajson'
		Please contribute your platform's build instructions. Thank you!

### Make: Fleece Tests

	  make test
		runs minimal test scripts that check the health of the fleece build

	  make test2
		runs some randomly nested test data that demonstrates fleece' output

	  make test3
		runs minimal test scripts that check the health of the bundled JSON4 and LuaJSON build
		to test bundled converters individually, use test-json4 or test-luajson

	  make test-5M
		runs 5,000,000 random nested tables, same as test2 but longer

	  make test-nonstop
		runs randomly nested test tables, same as test2 but runs forever

	  make unit-tests
		runs all unit tests, takes some minutes

	  make basic-unit-test
		very quick, basic unit tests

	  make complex-unit-test
		many complex, generated, deeply nested tables, mainly to test string buffers

	  make escape-unit-test parameter-unit-test
		various strings with control chars, through all escape settings

	  make parameter-unit-test
		testing parameters to the json() call, so far only E, E0-4

### Make: Benchmark Execution

	  make bench
		runs brief benchmark scripts for integers, strings, in arrays or objects.
		if you are getting 'sample too small, could not measure', use bench-10k

	  make bench2
		comparison to the native Lua JSON converter package JSON4
		JSON4 comes bundled with Fleece, you need not do any installing to run this
		if you are getting 'sample too small, could not measure', use bench2-1k

	  make bench3
		comparison to the native Lua JSON4 and the C library LuaJSON
		build LuaJSON first: 'make <PLATFORM>-test', see above
		if you are getting 'sample too small, could not measure', use bench3-1k
		if you are seeing crashes, try bench3a and bench3b to find which package is at fault

	  make bench4
		comparison to the native JSON4 and the C libraries LuaJSON and Lua-Yajl
		you need to download & build Lua-Yajl yourself, see test/README.md

	  make bench5a
		Crash test, Fleece only, same benchmarks but much longer running.
		bench5 also include JSON4 and LuaJSON, it's for comparison, but less relevant
		bench5b runs ONLY JSON4 and LuaJSON, not Fleece, to pinpoint causes of possible crashes

	  make bench6a
		obsolete

	  make bench7
		performance comparison of escape modes, vs JSON4 and LuaJSON. This lists all escape
		settings and loops through a declining frequency of control characters
		if you are getting 'sample too small, could not measure', use bench7-1k
		for speedier testing, bench7a tests only Fleece, bench7c only Fleece and LuaJSON

Please try `make test`, `make bench` and let us know your results. Thank you!

# Files

Files in the test/ directory. Use them with make, see next section, 'Make Options'.

### Tests

	test-fleece-hello.lua    most simple check if the fleece library can be loaded
	test-fleece-json.lua     converts the minimal-table {"ok"}
	test-fleece-rand.lua     produces, converts and prints five random tables
	test-fleece-size.lua     not in use yet: check the fleece.size(t) function

    test-fleece-5m.lua       crash test by 5 million random, deeply nested tables
    test-fleece-nonstop.lua  same as test-fleece-5m.lua but unlimited tables

	test-luajson.lua         check if LuaJSON is working (bundled for comparison)
    test-json4.lua           check if JSON4 is working (bundled for comparison)
 
### Unit Tests

	unit/                  unit tests, use `make unit-tests`
        test.lua           not used
		basic.lua          fast, basic unit tests 
		complex.lua        long, generated unit tests taking buffers to task
        escapes.lua        tests for the various escape options
        parameters.lua     testing parameters, currently only E, E0-4

### Benchmarks

The benchmarks are for Fleece only, or for comparing to a native Lua implementation of JSON, and to a C library implementation.  

Use them by executing make from the command line, see next sections 'Make Options'.

	bench-fleece.lua       benchmarks giving nanoseconds per table element for Fleece encoding
    bench-fleece-10k.lua   same as bench-fleece.lua but 10,000 instead of 1,000 cycles
	bench-json4.lua        benchmarks giving nanoseconds per table element for JSON4 native encoding
	bench-luajson.lua      benchmarks giving nanoseconds per table element for luajson C lib encoding
	bench-yajl.lua         benchmarks giving nanoseconds per table element for Lua-Yajl C lib encoding

	bench2.lua             benchmarks Fleece vs JSON4 (native Lua) encoding
    bench2-1k.lua          same as bench2.lua but 1,000 instead of 100 cycles
    
    bench3.lua             benchmarks Fleece vs JSON4 (native Lua) and luajson C lib encoding*
    bench3-1k.lua          same as bench3.lua but 1,000 instead of 100 cycles*
	bench3a.lua            same as bench3.lua but Fleece only
	bench3b.lua            same as bench3.lua but without Fleece*

	bench4.lua             benchmarks Fleece encoding vs JSON4 (native, and C libs luajson & Lua-Yajl*

	bench5.lua             same as bench3.lua but with very long run time*
	bench5a.lua            same as bench5, but Fleece only
	bench5b.lua            same as bench5, but JSON4 and LuaJSON only
	bench5c.lua            same as bench5, LuaJSON only*

    bench6a.lua            obsolete, was used to find a bug

    bench7.lua             escape character benchmarks, Fleece vs LuaJSON and JSON4
    bench7-1k.lua          same as bench7.lua, but with 1,000 instead 100 cycles
    bench7a.lua            same as bench7.lua, but Fleece only
    bench7c.lua            same as bench7.lua, but Fleece and LuaJSON only

	bench.lua              advises to use bench-fleece.lua as starting point

### Other files

    README.md              this file
    README.html            same as README.md, in html format 

(*) need making of luajson and or lua-yajl to work.

____________________________________________________________________________
README fleece/test hd Mar 2011 - powered by Eonblast <http://www.eonblast.com>
