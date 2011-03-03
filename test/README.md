<div class=head></div>

**Eonblast Fleece 0.2.2**

Tests, Benchmarks, Comparison
-----------------------------

This folder offers benchmarks and supports comparison of Fleece with:

* **JSON4 0.9.50**: aka JSON4Lua, JSON for Lua, module name 'json' http://json.luaforge.net/
* **LuaJSON 1.1**: aka luajsonlib, LuaJSON C Library http://luaforge.net/projects/luajsonlib/
* **Lua-Yajl 1.0.9**: with Oct '10  bindings https://github.com/brimworks/lua-yajl/

Note that the names of the various Lua-JSON packages can be confusing. They are not unique and therefore 'JSON4' and 'LuaJSON' are chosen here to clarify which packages are meant. They don't always call themselves this way.

JSON4 is native Lua, LuaJSON and Lua-Yajl are C libraries for Lua and thus faster. JSON4 is listed as 'cmjjson' with the comparisons at http://lua-users.org/wiki/JsonModules. 

If somebody has the bench code behind the numbers at http://lua-users.org/wiki/JsonModules, please let me know.

LuaJSON needed slight modifications for me to compile, see /* hd */ tags int the source. It sometimes crashes for me on one Mac, not sure why. To take the blame from Fleece, try bench3a then. Luajson comes with this package, in etc/luajson and can be made with 'make &lt;PLATFORM>-test'.
 
Installing Lua-Yajl is no real fun and you may want to skip that if your interest in the benchmarks is casual. There is no support for installing lua-yajl. You'll miss out on bench4, 5 and 5c but that is no real loss as Lua-Yajl beforms by and large similar to LuaJSON. Make sure to run 5a once and let me know if you ran into trouble (an eventual out of memory is ok).

	test-fleece-hello.lua  Most simple check if fleece is working
	test-fleece-json.lua   Converts a minimal-table
	test-fleece-rand.lua   Produces, converts and prints five tables
	test-fleece-size.lua   Check the fleece.size(t) function that predicts the JSON string length
	test-luajson.lua       Check if luajson is working (for comparison, this is not fleece itself)

	bench.lua              nop file giving the advice to use bench-fleece.lua
	bench-fleece.lua       benchmarks in nanoseconds per table element for Fleece encoding
	bench-json4.lua        benchmarks in nanoseconds per table element for JSON4 native encoding
	bench-luajson.lua      benchmarks in nanoseconds per table element for luajson C lib encoding
	bench-yajl.lua         benchmarks in nanoseconds per table element for Lua-Yajl C lib encoding
	bench2.lua             benchmarks Fleece vs JSON4 (native Lua) encoding
	bench3.lua             benchmarks Fleece vs JSON4 (native Lua) and luajson C lib encoding*
	bench3a.lua            benchmarks as before but Fleece only (mainly if 3 crashes to find why)
	bench3b.lua            benchmarks as before without Fleece  (mainly if 3 crashes to find why)*
	bench4.lua             benchmarks Fleece encoding vs JSON4 (native, and C libs luajson & Lua-Yajl*
	bench5.lua             benchmarks Fleece like before but with very long run time*
	bench5a.lua            benchmarks as before, very long run time, Fleece only
	bench5b.lua            benchmarks as before, very long run time, JSON4 only
	bench5c.lua            benchmarks as before, very long run time, luajson C lib only*

(*) need making of luajson and or lua-yajl to work.

E.g. running 

	make macosx-test
	make bench3
	
will print something like this:
	
	lua test/bench3.lua
	Fleece Benchmarks vs Json4 / native Lua and LuaJSON C lib
	=========================================================
	A couple of random tables are created and speed is clocked.
	You should have built fleece first with 'make <PLATFORM>', 
	and also built luajson.so with 'make <PLATFORM>-test,
	and now be in the fleece root directory.
	LUAJSON (NOT FLEECE) SOMETIMES CRASHES THIS TEST. SEE BENCH3a and BENCH3b.
	Lua 5.1 official
	---------------------------------------------------------------------------------
	1000 elements in t[i]=i                   
	100x luajson.stringify(t)        3000ns/element           [1,2,3,4,5,6,7,8,9,1.. 
	100x json4.encode(t)             7800ns/element           [1,2,3,4,5,6,7,8,9,1.. 
	100x fleece.json(t)               300ns/element 10%,  3%  [1,2,3,4,5,6,7,8,9,1.. 
	---------------------------------------------------------------------------------
	1000 elements in t['x'..i]=i              
	100x luajson.stringify(t)        4700ns/element           {"x97":97,"x366":366.. 
	100x json4.encode(t)            10600ns/element           {"x97":97,"x366":366.. 
	100x fleece.json(t)               400ns/element  8%,  3%  {"x518":518,"x744":7.. 
	---------------------------------------------------------------------------------
	1000 elements in t[i]=randstr(i)          
	100x luajson.stringify(t)        2800ns/element           ["d","lnfbrryjnvabnr.. 
	100x json4.encode(t)             9000ns/element           ["otkxtbnyemkrqdcyhh.. 
	100x fleece.json(t)               200ns/element  7%,  2%  ["fgyqrqmmvkuouatzye.. 
	---------------------------------------------------------------------------------
	1000 elements in t[randstr(i)]=randstr(i) 
	100x luajson.stringify(t)        5300ns/element           {"bbwd":"kkpgojbhnor.. 
	100x json4.encode(t)            12600ns/element           {"wvubdmsybdgtycqjg".. 
	100x fleece.json(t)               500ns/element  9%,  3%  {"hfaxoxd":"yyndsgxy.. 
	---------------------------------------------------------------------------------
	Note that fleece may list associative arrays in different order.
	

<pre>
<style>
div.head { border: 0; border-top-left-radius: 7px; 
           border-bottom-right-radius: 15px; 
           background: url(../etc/images/fleece-footer-1.1.png) no-repeat top right;
           height:81px; padding: 0; margin: 0; }  
div.foot { border: 0; border-bottom-left-radius: 15px; 
           border-bottom-right-radius: 15px; 
           background: url(../etc/images/fleece-footer-1.1.png) no-repeat bottom left;
           height:31px; padding: 0; margin: 0; }  
</style>
</pre>
____________________________________________________________________________
README fleece/test hd Feb 2011 - powered by Eonblast http://www.eonblast.com

<div class=foot></div>
