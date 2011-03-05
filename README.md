<div class=head></div>

# Eonblast Fleece 0.2.3 Lite

**Fast Lua to JSON conversion.**

Fleece is optimized for fastest Lua to JSON conversion and beats other JSON packages by around 10 times.  I would be happy if you found that something new became possible with one of your projects thanks to Fleece' speed.

Fleece is produced for Eonblast's Eonbeam game servers to address a performance bottle neck.  Fleece is a one-way affair: Lua to JSON, and it will remain like this for the time being. It should get additional specialized tuning parameters. 

Please send any kind of feedback my way, bugs, docs, source, comments. Thank you! 

**THIS IS AN ALPHA RELEASE, SOME ESSENTIALS ARE MISSING**

**Download:**

	git clone https://Eonblast@github.com/Eonblast/Fleece-Lite.git fleece

**Build**, at this point, Linux and Mac OS X only:

	make linux # or make macosx, or make macosx-old for <= 10.3

Please contribute your platform's make instructions, thanks.

**Testing:**

	make test
	make unit-test # long

**Benchmarking:**

	make bench
	make bench2
	make macosx-test # or linux-test, or macosx-old-test
	make bench3

**Sample:**

	t = {1,2,3}
	json_str = fleece.json(t)
	
Fleece works with Lua 5.1.4 and 5.1.4-2, it can be adapted to other Lua versions or LuaJIT and will be ported to 5.2. Part of its speed advantage over other JSON packages is derived from the fact that it accesses the Lua table data at the core C level, below the Lua C API, which frees a significant number of cycles. It also uses a string buffer that is custom designed for the specific task of building a JSON string. It uses custom programmed, faster float and integer conversion and at the expense of some memory consumtion, it keeps some string and lookup buffers around once initialized, for the next encoding. Performance can be tuned by tweaking buffer sizes at compile time, by setting defines in src/fleece-intern.h. The defaults are set for Fleece to produce standard JSON reasonably fast.

## Functions

**fleece.json()** - convert (stringify) a Lua value to a JSON string  
**fleece.size()** - dry run to find the JSON result size, without creating it  

## Road Map 

Could look like this:

0.3.1  

* Official JSON compatibility, escapes etc.  
* **fleece.warp()** - convert (stringify) a Lua value to a JSON string,  fastest.  
* **fleece.pack()** - convert (stringify) a Lua value to a JSON string, smallest.  

0.3.2  

* **fleece.time()** - dry run to find a cpu time estimate for a conversion  
* **fleece.buff()** - dry run to find the memory needed, without allocating it  


#### Objects and Arrays

Lua knows only tables, JSON knows arrays and objects, with sparse arrays possible using 'null'. Those JSON objects and arrays are produced from one and the same Lua table type and for any conversion, a decision must be found to weigh output size, speed and memory usage. 

## Branches

This is the open 'lite' branch, the master is cluttered and not open at this point. Please check the license below. As natural person you can use it without restriction. Same for anything Open Source. 

This License is valid for Fleece Lite. It is not applicable for the master branch, in case we should share it with you and it is not applicable for JSON4 and LuaJSON that are bundled with Fleece Lite for comparison, in etc/.

## History

**0.2.3 lite** - 05 Mar 2011 - hd  

* added unit tests, basic and complex, generated cases  
* major testing for special cases running over buffer boundaries  
* fixed buffer overrun on consecutive brackets  
* buffer functions and macros weeded out and simplified  
* verbosity and selftest cond compiles added  
* unified verbosity and selftest definition  
* added main config header  
  
**0.2.2 lite** - 03 Mar 2011 - hd  

* first pulished version  


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

<pre>
<style>
div.head { border: 0; border-top-left-radius: 7px; 
           border-bottom-right-radius: 15px; 
           xbackground: url(http://www.eonblast.com/images/fleece-footer-1.1.png) no-repeat top right;
           background: url(etc/images/fleece-footer-1.1.png) no-repeat top right;
           height:81px; padding: 0; margin: 0; }  
div.foot { border: 0; border-bottom-left-radius: 15px; 
           border-bottom-right-radius: 15px; 
           background: url(etc/images/fleece-footer-1.1.png) no-repeat bottom left;
           height:31px; padding: 0; margin: 0; }  
</style>
</pre>
_______________________________________________________________________
README fleece hd Feb 2011 - powered by Eonblast http://www.eonblast.com

<div class=foot></div>
