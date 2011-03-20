# Eonblast Fleece 0.3.1 Lite

**Fast Lua to JSON conversion.**

Fleece is optimized for fastest Lua to JSON conversion and beats other JSON packages by around 10 times.  I would be happy if you found that something new became possible with one of your projects thanks to Fleece' speed.

Fleece is produced for Eonblast's Eonbeam game servers to address a performance bottle neck.  Fleece is a one-way affair: Lua to JSON, and it will remain like this for the time being. It will likely acquire additional tuning parameters. 

Please send any kind of feedback my way: bugs, docs, source, comments, random thoughts. If you have a platform other than Linux or Mac, please contribute your make instructions (for src/Makefile). Thank you! 

**Download:**

	git clone https://Eonblast@github.com/Eonblast/Fleece-Lite.git fleece

**Build**, at this point, Linux and Mac OS X only:

	cd fleece
	make linux # or linux-64, macosx, or make macosx-old (for <= 10.3)

Please contribute your platform's make instructions, thanks.


**Testing:**

	make test
	make test2
	make check


**Benchmarking:**

	make bench
	make bench2
	make macosx-test # or linux-test, linux-64-test or macosx-old-test
	make bench3


**Sample:**

	t = {1,2,3}
	json_str = fleece.json(t)


**Compatibility**

Fleece works with Lua 5.1.4 and 5.1.4-2, it can be adapted to other Lua versions or LuaJIT and will be ported to 5.2. 

**Performance**

Part of Fleece' speed advantage over other JSON packages is derived from the fact that it accesses the Lua table data at the core C level, below the Lua C API, which frees a significant number of cycles. It also uses string buffers that are custom designed for the specific task of building a JSON string. It can employ custom programmed, faster float and integer conversion. And at the expense of some memory consumtion, it keeps one string buffer and two lookup tables around once initialized, for the next encoding. 

If you need proper JSON character escaping and are on a x86 platform, Fleece can power up inline assembler to make the string escaping sufficiently fast. If you have your strings in check and can free Fleece from taking care of the escaping, the C string routines are just as fast as the inline assembler. See 'Escape Flags', below.

Performance can be tuned by tweaking buffer sizes at compile time, i.e. setting defines in src/fleece-intern.h. The expectation is that there will be more parameters in the future that will allow to control the performance and trade offs directly. The defaults out of the box are set for Fleece to produce standard JSON as fast as possible. 

Performance can be checked using 'make bench', interesting are bench2, bench3 and bench7.

## Functions

Fleece currently exposes one function:

**fleece.json()** - convert (stringify) a Lua value to a JSON string  

## Sample

    require("fleece")    
    str = fleece.json({"Hello", 2, "Json"})
    print(str)
    --> ["Hello", 2, "Json"]

## json() Parameters

#### 1: Lua Data

The first parameter to fleece.json() is the Lua data, often a table, that should be converted into a JSON string.

#### 2: Escape Flags

The second parameter is optional, it allows you to control how the JSON string  should be constructed in the event that ASCII control characters are encountered in Lua strings in the input. 

    print(fleece.json("he\\o!"))        --> "he\o!"
    print(fleece.json("he\\o!", "E1))   --> "he*o!"
    print(fleece.json("he//o!", "E3"))  --> "he\/\/o!"  

JSON regards as control characters ASCII **0 - 31** and **127**, it excpects **"** and **\\** escaped and allows to escape **/** to avoid `</`, which can pose problems in HTML strings. Escaping means prepending a **\\** or using the **\uNNNN** notation.

To comply with the JSON standard, all those characters should be escaped. That comes with a performance penalty. So if you have good control over your data and know it can't contain funky chars, you want to leave escaping switched off. That is the default and equivalent to using **E0**. You can check out the panelty on your machine by using 'make bench7a'.

There is no panelty, with any of the flags, if you encode only numbers, bools and tables. So if an overwhelming majority of your data consists of numbers, you could just as well play it very safe and use **E4** and be covered for any eventuality. But, e.g., if you are using string keys and are certain that they don't contain any control characters, and not **"** or **\\**, you should remain with the default, i.e. use **E0** or set no E-flag, to get maximal performance.

The second fastest option is to 'purge' the worst two characters, **"** and **\\**. This is what **E1** does. They are the worst, because they can break the JSON string, cut it in two and make the resulting JSON output having an invalid format. Obviously, a not escaped, stray **"** as 'part of a string' will do exactly that. The flag **E1** makes sure that they are replaced by a *****. You will be able to change the purge character by changing FLEECE\_PURGE\_CHAR in src/fleece-config.h in a later version of Fleece. This is a crude solution, but it gives safety for a penalty of about 25-50%, depending on how many escaped characters are encountered. The more, the slower. And if you are using assembler, the penalty will be absolutely and relatively lower, of 10-15%.

To escape only **"** and **\\**, i.e. turning them into **\"** and **\\\\** respectively, costs more time. It's the flag **E2** that triggers the conversion and the penalty can be 50-100%; again less when using assembler. So this means that it can take up to double the time to create the resulting JSON string as when using **E0**. And the more control characters are encountered, the slower the encoding. 

The **E3** flag includes a third character, **/** into this special treatment, which is converted to **\/**, to make sure that HTML is not violated. This can add another 20% slow down in cases with very high frequency of control characters. Again, use bench7a to check it out.

Finally, **E4**, or simply **E**, converts all characters, following the standard, especially the control characters <= 31, as prescribed in the JSON spec. Most characters from 0 through 31, and 127, are translated into a Unicode hex notation of **\uNNNN**. E.g. \u000b for control character #11. But 8, 9, 10, 12, 13 are made into **\f**, **\b**, **\r**, **\t**, **\n** respectively. **"**, **\\** and **/** are escaped to **\"**, **\\\\**, **\/**, just as **E3** does. This is the slowest variant. It can make things 80-250% percent slower, the worse, the more control characters need to be escaped. If they are few and far between, there is not much difference between the time that E1, E2, E3 and E4 take, and that is going to be around 50% over the time of E0, i.e. no escaping. This means, e.g. instead of 300ns per element, 500ns might be needed, on average, for strings. As mentioned before, numbers, booleans and tables per se  will not slow things down; only strings.

Especially for escaping, you will benefit from using one of the -asm build variants, if you can. Loss of speed will be much lower, in both absolute and relative terms. It will be only 10-20%. E.g.: 80ns/element for E0, and 90ns/element for E4.

* **E:** same as E4: escape all, according to JSON standarrd.
* **E0:** no escaping, the default. Way fastest but can produce invalid JSON.
* **E1:** replacement of " and \ with *.
* **E2:** escaping " and \ to \" and \\.
* **E3:** escaping ",\,/ to \", \\ and \/.
* **E4:** escaping 0-31, 127, ", \ and /. 0 -> \u0000 etc. 8 -> \b etc.

#### Benchmarking

You can check the effects the escape flags have on performance by using

	make bench7a # Fleece only
	make bench7  # Fleece vs JSON4, LuaJSON

For the latter you'll see something like this:
	  
	  Frequency of escape characters 1 in 100
	  ---------------------------------------------------------------------------------
	  1000 elements in t[i]=randstr(i)          
	  100x luajson.stringify(t)         767ns/element              ["wnj","ukhclqiy","zfbvknygrw","yrpidgqb","vrlkqjjox","iwsj".. 
	  100x json4.encode(t)             3873ns/element              ["wnj","ukhclqiy","zfbvknygrw","yrpidgqb","vrlkqjjox","iwsj".. 
	  100x fleece.json(t, "E0")          80ns/element  10%,   2%   ["wnj","ukhclqiy","zfbvknygrw","yrpidgqb","vrlkqjjox","iwsj".. 
	  100x fleece.json(t, "E1")         128ns/element  16%,   3%   ["wnj","ukhclqiy","zfbvknygrw","yrpidgqb","vrlkqjjox","iwsj".. 
	  100x fleece.json(t, "E2")         127ns/element  16%,   3%   ["wnj","ukhclqiy","zfbvknygrw","yrpidgqb","vrlkqjjox","iwsj".. 
	  100x fleece.json(t, "E3")         126ns/element  16%,   3%   ["wnj","ukhclqiy","zfbvknygrw","yrpidgqb","vrlkqjjox","iwsj".. 
	  100x fleece.json(t, "E4")         134ns/element  17%,   3%   ["wnj","ukhclqiy","zfbvknygrw","yrpidgqb","vrlkqjjox","iwsj".. 
	  ---------------------------------------------------------------------------------
	  1000 elements in t[randstr(i)]=randstr(i) 
	  100x luajson.stringify(t)        1288ns/element              {"wjj":"ygdwzvvcsx","nhfo":"puaefl","oo\u0001y":"k","svjbhuz.. 
	  100x json4.encode(t)             5040ns/element              {"wjj":"ygdwzvvcsx","nhfo":"puaefl","ooy":"k","svjbhuzc":"z.. 
	  100x fleece.json(t, "E0")         146ns/element  11%, 2.8%   {"tjt":"ixxv","jt":"	nw","cafioibhh":"np","gmw":"prgksqle",".. 
	  100x fleece.json(t, "E1")         245ns/element  19%,   4%   {"tjt":"ixxv","jt":"	nw","cafioibhh":"np","gmw":"prgksqle",".. 
	  100x fleece.json(t, "E2")         235ns/element  18%,   4%   {"tjt":"ixxv","jt":"	nw","cafioibhh":"np","gmw":"prgksqle",".. 
	  100x fleece.json(t, "E3")         237ns/element  18%,   4%   {"tjt":"ixxv","jt":"	nw","cafioibhh":"np","gmw":"prgksqle",".. 
	  100x fleece.json(t, "E4")         256ns/element  19%,   5%   {"tjt":"ixxv","jt":"\tnw","cafioibhh":"np","gmw":"prgksqle",.. 
 

But if you used an assembler build target, it might look like this:

	  Frequency of escape characters 1 in 100
	  ---------------------------------------------------------------------------------
	  1000 elements in t[i]=randstr(i)          
	  100x luajson.stringify(t)         758ns/element              ["ei","beir","mwwxnl","uh","ufjaexlizf","slmxda","k","vwtm",.. 
	  100x json4.encode(t)             3857ns/element              ["ei","beir","mwwxnl","uh","ufjaexlizf","slmxda","k","vwtm",.. 
	  100x fleece.json(t, "E0")          80ns/element  10%,   2%   ["ei","beir","mwwxnl","uh","ufjaexlizf","slmxda","k","vwtm",.. 
	  100x fleece.json(t, "E1")          89ns/element  11%, 2.3%   ["ei","beir","mwwxnl","uh","ufjaexlizf","slmxda","k","vwtm",.. 
	  100x fleece.json(t, "E2")          90ns/element  11%, 2.3%   ["ei","beir","mwwxnl","uh","ufjaexlizf","slmxda","k","vwtm",.. 
	  100x fleece.json(t, "E3")          91ns/element  11%, 2.3%   ["ei","beir","mwwxnl","uh","ufjaexlizf","slmxda","k","vwtm",.. 
	  100x fleece.json(t, "E4")          92ns/element  12%, 2.3%   ["ei","beir","mwwxnl","uh","ufjaexlizf","slmxda","k","vwtm",.. 
	  ---------------------------------------------------------------------------------
	  1000 elements in t[randstr(i)]=randstr(i) 
	  100x luajson.stringify(t)        1219ns/element              {"ctvogwgix":"cqqjafab","ckg":"pjegxgdwj","vudnegkfjp":"cgyg.. 
	  100x json4.encode(t)             4778ns/element              {"ctvogwgix":"cqqjafab","ckg":"pjegxgdwj","vudnegkfjp":"cgyg.. 
	  se","".. ce.json(t, "E0")         141ns/element  11%, 2.9%   {"ric":"vyzcvnx","tdaay":"kxzcbcbugz","kqsooc":"gfdxj
	  se","*.. ce.json(t, "E1")         159ns/element  13%,   3%   {"ric":"vyzcvnx","tdaay":"kxzcbcbugz","kqsooc":"gfdxj
	  se","\.. ce.json(t, "E2")         161ns/element  13%,   3%   {"ric":"vyzcvnx","tdaay":"kxzcbcbugz","kqsooc":"gfdxj
	  se","\.. ce.json(t, "E3")         158ns/element  12%,   3%   {"ric":"vyzcvnx","tdaay":"kxzcbcbugz","kqsooc":"gfdxj
	  100x fleece.json(t, "E4")         161ns/element  13%,   3%   {"ric":"vyzcvnx","tdaay":"kxzcbcbugz","kqsooc":"gfdxj\rse",".. 

Note that the left side is mangled due to non-escaped control characters being dumped on the screen. Which is expected behavior for E0 - E3.

#### json(): Return Value

The JSON string.


## Contribution

#### Platform Build Instructions
Please contribute your platform's build instructions for src/Makefile.

#### Crash Testing  
Please try to crash Fleece using  

* make complex-unit-test  
* make bench5  
* make test-5m  

## Branches

This is the open 'lite' branch, the master is cluttered and not open at this point. Please check the license below. As natural person you can use it without restriction. Same for anything Open Source. 

This License is valid for Fleece Lite. It is not applicable for the master branch, in case we should share it with you and it is not applicable for JSON4,  LuaJSON, Lua-Yajl, Yajl, lunit or Markdown.lua that are bundled with Fleece Lite for comparison and support, in the etc folder.

## History

**0.3.1 lite** - 20 Mar 2011 - hd  

* added missing header in include/
* added more benchmarks for Lua-Yajl
* added benchmark sample page

**0.3.0 lite** - 19 Mar 2011 - hd  

* optimized C count, escape and copy code
* optimized x86 assembler escape and copy code
* make options and flag for using assembler or c copy code
* flag for using strcpy for memcpy
* make file, and make doc extension for bench and test options
* source re-organization and awk clean up script
* additional benchmark and unit test scripts
* enhanced docs

**0.2.5 lite** - 09 Mar 2011 - hd  

* escape characters 0-31, 127, " \ /
* escape flags E, E0, E1, E2, E3, E4 in function call
* escape unit tests (make escape-unit-test) and benchmarks (make bench7)

**0.2.4 lite** - 06 Mar 2011 - hd  

* added linux 64 bit make instructions  
* fixed memory corruption on 64bit Linux  
* added buffer end sanity tag  
* enhanced formatting of html pages  
* added luajson to etc  

**0.2.3 lite** - 05 Mar 2011 - hd  

* added unit tests, basic and complex, generated cases  
* major testing for special cases running over buffer boundaries  
* fixed buffer overrun on consecutive brackets  
* buffer functions and macros weeded out and simplified  
* verbosity and selftest cond compiles added  
* unified verbosity and selftest definition  
* added main config header  
* added lunit to etc
  
**0.2.2 lite** - 03 Mar 2011 - hd  

* first pulished version  

## Credits

Thanks for testing and suggesting improvements:

**[Vadim Peretokin](https://github.com/vadi2)**  
**[Etan Reisner](https://github.com/deryni)**  

Thanks to these contributors to the Lua ecosystem:

**Nathaniel Musgrove: [LuaJSON](http://luaforge.net/projects/luajsonlib/)**  
Serving as starting point and continuous reference during development

**Craig Mason-Jones: [JSON4Lua](http://json.luaforge.net/)**  
The complentary, easiest and most robust solution around

**Michael Roth: [lunit](http://www.nessie.de/mroth/lunit/)**  
Straight forward Unit Testing

**Niklas Frykholm: [Markdown.lua](http://www.frykholm.se/files/markdown.lua)**  
Invaluable preview and doc tool


## License

**Copyright (c) 2011 Eonblast Corporation**

Permission is  hereby  granted,  free of charge,  to  any person
obtaining  a copy of  this software and associated documentation
files  (the  "Software"),  to  evaluate  the Software  or use it
in education,  science,  not-for-profit work,  personal, or Open
Source projects including without limitation the rights  to copy,
modify, merge, publish, distribute,  but not sublicense, and not
sell copies of the Software, subject to the following conditions:

The above copyright notice and this  permission notice  shall be
included  in all copies or substantial portions of  the Software.

THE SOFTWARE IS PROVIDED  "AS IS",  WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO  THE WARRANTIES
OF  MERCHANTABILITY,   FITNESS  FOR  A  PARTICULAR  PURPOSE  AND
NONINFRINGEMENT.  IN  NO  EVENT  SHALL  THE AUTHORS OR COPYRIGHT
HOLDERS  BE  LIABLE FOR  ANY CLAIM,  DAMAGES OR  OTHER LIABILITY,
WHETHER IN AN ACTION OF  CONTRACT,  TORT  OR OTHERWISE,  ARISING
FROM,  OUT OF OR  IN CONNECTION WITH THE SOFTWARE  OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

_______________________________________________________________________
README fleece hd Mar 2011 - powered by Eonblast http://www.eonblast.com
