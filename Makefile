###-------------------------------------------------------------------------###
### Package     : Fleece - fast Lua to JSON conversion                      ###
### File        : ./Makefile                                                ###
### Description : Main make file, building, tests, benchmarks               ###
### Version     : 0.3.1 / alpha                                             ###
### Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ###
### Author      : H. Diedrich <hd2010@eonblast.com>                         ###
### License     : see file LICENSE                                          ###
### Created     :    Feb 2011                                               ###
### Changed     : 19 Mar 2011                                               ###
###-------------------------------------------------------------------------###
###                                                                         ###
###  Fleece is optimized for the fastest Lua to JSON conversion and beats   ###
###  other JSON implementations by around 10 times, native Lua up to 100x.  ###
###                                                                         ###
###-------------------------------------------------------------------------###
###                                                                         ###
###  Also see src/Makefile, src/fleece-intern.h and etc/luajson/Makefile.   ###
###                                                                         ###
###  PLEASE CONTRIBUTE YOUR *NIX, WIN OR OTHER PLATFORM'S INSTRUCTIONS      ###
###  for src/Makefile and etc/luajson/Makefile. Thank you!                  ###
###                                                                         ###
###-------------------------------------------------------------------------###
###                                                                         ###
###  Build: make <PLATFORM>                                                 ###
###  Help:  make                                                            ###
###  Test:  make test                                                       ###
###  Bench: make bench                                                      ###
###                                                                         ###
###  PLATFORMS: linux linux-asm linux-64 linux-64-asm                       ###
###             macosx macosx-asm macosx-old                                ###
###                                                                         ###
###  More description, and many more test* and bench* options:              ###
###  run 'make' without arguments for more info and a list of platforms.    ###
###                                                                         ###
###-------------------------------------------------------------------------###

# Your platform. See PLATS for possible values.
PLAT= none

all: $(PLAT)

# use one of these, e.g. 'make linux'
PLATS= linux linux-asm linux-64 linux-64-asm macosx macosx-asm macosx-old
# wish list: aix ansi bsd freebsd generic mingw posix solaris

# use one of these to build luajson for comparison
TPLATS= linux-test linux-64-test macosx-test macosx-old-test
# wish list: aix-test ansi-test bsd-test freebsd-test generic-test mingw-test posix-test solaris-test

# branch off to src/Makefile
$(PLATS):
	(cd src && $(MAKE) $@)

# make test comparison module luajson (not part of fleece)
$(TPLATS):
	(cd etc/luajson && $(MAKE) $@)

# get submodule source: lunit
# re git submodules: http://progit.org/book/ch6-6.html
lunit:
	git submodule init etc/lunit
	git submodule update etc/lunit

# simple tests
test:
	@lua test/test-fleece-hello.lua
	@lua test/test-fleece-json.lua
# lua test/test-fleece-size.lua

test2:
	lua test/test-fleece-rand.lua

test3:
	lua test/test-json4.lua
	lua test/test-luajson.lua
	# lua test/test-yajl.lua

test-json4:
	@echo this should run out of the box
	lua test/test-json4.lua

test-luajson:
	@echo this will run only after building luajson, 
	@echo by 'make <PLATFORM>-test', do 'make' for details 
	lua test/test-luajson.lua

test-5m:
	# crash test, 5,000,000 random tables
	lua test/test-fleece-5m.lua

test-nonstop:
	# positively endless random tables
	lua test/test-fleece-nonstop.lua

bench:
	lua test/bench-fleece.lua
	@echo Also see test/README.md

bench-10k:
	lua test/bench-fleece-10k.lua
	@echo Also see test/README.md

bench2:
	lua test/bench2.lua
	@echo Also see test/README.md

bench2-1k:
	lua test/bench2-1k.lua
	@echo Also see test/README.md

bench3:
	lua test/bench3.lua
	@echo Also see test/README.md

bench3-1k:
	lua test/bench3-1k.lua
	@echo Also see test/README.md

bench3a:
	lua test/bench3a.lua
	@echo Also see test/README.md

bench3b:
	lua test/bench3b.lua
	@echo Also see test/README.md

bench4:
	lua test/bench4.lua
	@echo Also see test/README.md

bench5:
	lua test/bench5.lua
	@echo Also see test/README.md

bench5a:
	lua test/bench5a.lua
	@echo Also see test/README.md

bench5b:
	lua test/bench5b.lua
	@echo Also see test/README.md

bench6a: # mostly irrelevant now: specific bug hunt test
	lua test/bench6a.lua
	@echo Also see test/README.md

bench7: # Fleece, JSON4 & LuaJSON
	lua test/bench7.lua
	@echo Also see test/README.md

bench7-1k: # finer granulty than 7 by virtue of a bigger sample 
	lua test/bench7-1k.lua
	@echo Also see test/README.md

bench7a: # same as 7 but only Fleece 
	lua test/bench7a.lua
	@echo Also see test/README.md

bench7c: # same as 7 but only Fleece & LuaJSON (faster results)
	lua test/bench7c.lua
	@echo Also see test/README.md

basic-unit-test:
	@echo "======================="
	@echo "Fleece Basic Unit Tests"
	@echo "======================="
	make lunit
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/basic.lua

parameter-unit-test:
	@echo "============================"
	@echo "Fleece Parameters Unit Tests"
	@echo "============================"
	make lunit
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/parameters.lua

escape-unit-test:
	@echo "============================="
	@echo "Fleece Escape Code Unit Tests"
	@echo "============================="
	make lunit
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/escapes.lua

complex-unit-test:
	@echo "========================="
	@echo "Fleece Copmlex Unit Tests"
	@echo "========================="
	make lunit
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/complex.lua

unit-test:
	@echo "================"
	@echo "Retrieving lunit"
	@echo "================"
	make lunit
	@echo "======================="
	@echo "Fleece Basic Unit Tests"
	@echo "======================="
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/basic.lua
	@sleep 1
	@echo "============================"
	@echo "Fleece Parameters Unit Tests"
	@echo "============================"
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/parameters.lua
	@sleep 1
	@echo "============================="
	@echo "Fleece Escape Code Unit Tests"
	@echo "============================="
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/escapes.lua
	@sleep 1
	@echo "========================="
	@echo "Fleece Copmlex Unit Tests"
	@echo "========================="
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/complex.lua

check:
	@echo "============================="
	@echo "C H E C K   U P"
	@echo "============================="
	@echo "brief but comprehensive tests"
	@echo "============================="
	@echo "Simple Tests"
	@echo "============================="
	make test
	make test2
	@echo "============================="
	@echo "Simple Benchmarks"
	@echo "============================="
	make bench
	make bench2
	make bench7a
	@echo "============================="
	@echo "Retrieving lunit"
	@echo "============================="
	make lunit
	@echo "============================="
	@echo "Fleece Basic Unit Tests"
	@echo "============================="
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/basic.lua
	@sleep 1
	@echo "============================"
	@echo "Fleece Parameters Unit Tests"
	@echo "============================"
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/parameters.lua
	@sleep 1
	@echo "============================="
	@echo "Fleece Escape Code Unit Tests"
	@echo "============================="
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/escapes.lua
	@echo "Looks good."


# edit to use for version number updates across all files
upversion:
	sed -E -e s/0\.3\.0/0.3.1/g -i .bak *.*
	sed -E -e s/0\.3\.0/0.3.1/g -i .bak Makefile
	sed -E -e s/0\.3\.0/0.3.1/g -i .bak test/*.*
	sed -E -e s/0\.3\.0/0.3.1/g -i .bak test/unit/*.*
	sed -E -e s/0\.3\.0/0.3.1/g -i .bak src/*.*
	sed -E -e s/0\.3\.0/0.3.1/g -i .bak etc/*.*
	sed -E -e s/0\.3\.0/0.3.1/g -i .bak include/*.*

# clean up *.bak files that are produced by 'make upversion'
delbak:
	rm -f *.bak
	rm -f */*.bak
	rm -f */*/*.bak

clean:
	cd src && $(MAKE) $@
	cd etc/luajson && $(MAKE) $@
	rm -f *.s *.o *.m *.M 

# produce a sanitized version of the source for fleece-lite
cleansrc: 
	rm -rf cleansrc
	mkdir cleansrc
	for file in src/*.c ; do awk -f etc/cleansrc.awk "$$file" | awk '/^[ 	]*$$/{ if (! blank++) print; next } { blank=0; print }' > "clean$$file" ; done
	for file in src/*.h ; do awk -f etc/cleansrc.awk "$$file" | awk '/^[ 	]*$$/{ if (! blank++) print; next } { blank=0; print }' > "clean$$file" ; done
	cp src/Makefile cleansrc
	cp src/README.* cleansrc

# produce doc html from readmes
html:
	@lua etc/markdown.lua --style etc/css/style.css --inline-style README.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(etc/g' -i .bak README.html
	@lua etc/markdown.lua --style etc/css/style.css --inline-style src/README.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(..\/etc/g' -i .bak src/README.html
	@sed -E -e 's/src=etc/src=..\/etc/g' -i .bak src/README.html
	@lua etc/markdown.lua --style etc/css/style.css --inline-style etc/README.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(..\/etc/g' -i .bak etc/README.html
	@sed -E -e 's/src=etc/src=..\/etc/g' -i .bak etc/README.html
	@lua etc/markdown.lua --style etc/css/style.css --inline-style test/README.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(..\/etc/g' -i .bak test/README.html
	@sed -E -e 's/src=etc/src=..\/etc/g' -i .bak test/README.html
	@lua etc/markdown.lua --style etc/css/style.css --inline-style test/SAMPLE.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(..\/etc/g' -i .bak test/SAMPLE.html
	@sed -E -e 's/src=etc/src=..\/etc/g' -i .bak test/SAMPLE.html
	@lua etc/markdown.lua --style etc/css/style.css --inline-style LICENSE
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(etc/g' -i .bak LICENSE.html
	@echo "done, start from ./index.html (which, itself is not generated)"

# print help
none:
	@echo 
	@echo "Building Fleece"
	@echo "---------------"
	@echo "To build Fleece, please do"
	@echo "    make <PLATFORM>"
	@echo 
	@echo "where <PLATFORM> is one of these:"
	@echo "    $(PLATS)"
	@echo 
	@echo "    e.g. 'make linux-64'"
	@echo 
	@echo "    *old: 'macosx-old' is for Mac OS X < 10.4 / PPC, see http://lua-users.org/wiki/BuildingModules"
	@echo "    *asm: The *-asm options work for x86 only and are using slightly faster assembler copy routines"
	@echo "    Please contribute your platform's make instructions. See Makefile, 'wish list'. Thank you!"
	@sleep 2
	
	@echo 
	@echo "Testing and Benchmarking"
	@echo "------------------------"
	@echo 
	@echo "	  $(TPLATS)"
	@echo "		these build the LuaJSON C lib, which comes bundled, for comparison"
	@echo "		test by using 'make test-luajson'"
	@echo "		Please contribute your platform's build instructions. Thank you!"
	@echo "	  test"
	@echo "		runs minimal test scripts that check the health of the fleece build"
	@echo "	  test2"
	@echo "		runs some randomly nested test data that demonstrates fleece' output"
	@echo "	  test3"
	@echo "		runs minimal test scripts that check the health of the bundled JSON4 and LuaJSON build"
	@echo "		to test bundled converters individually, use test-json4 or test-luajson"
	@echo "	  test-5M"
	@echo "		runs 5,000,000 random nested tables, same as test2 but longer"
	@echo "	  test-nonstop"
	@echo "		runs randomly nested test tables, same as test2 but runs forever"
	@sleep 1
	
	@echo 
	@echo "	  unit-tests"
	@echo "		runs all unit tests, takes some minutes"
	@echo "	  basic-unit-test"
	@echo "		very quick, basic unit tests"
	@echo "	  complex-unit-test"
	@echo "		many complex, generated, deeply nested tables, mainly to test string buffers"
	@echo "	  escape-unit-test parameter-unit-test"
	@echo "		various strings with control chars, through all escape settings"
	@echo "	  parameter-unit-test"
	@echo "		testing parameters to the json() call, so far only E, E0-4"
	@sleep 1
	
	@echo 
	@echo "	  bench"
	@echo "		runs brief benchmark scripts for integers, strings, in arrays or objects."
	@echo "		if you are getting 'sample too small, could not measure', use bench-10k"
	@echo "	  bench2"
	@echo "		comparison to the native Lua JSON converter package JSON4"
	@echo "		JSON4 comes bundled with Fleece, you need not do any installing to run this"
	@echo "		if you are getting 'sample too small, could not measure', use bench2-1k"
	@echo "	  bench3"
	@echo "		comparison to the native Lua JSON4 and the C library LuaJSON"
	@echo "		build LuaJSON first: 'make <PLATFORM>-test', see above"
	@echo "		if you are getting 'sample too small, could not measure', use bench3-1k"
	@echo "		if you are seeing crashes, try bench3a and bench3b to find which package is at fault"
	@echo "	  bench4"
	@echo "		comparison to the native JSON4 and the C libraries LuaJSON and Lua-Yajl"
	@echo "		you need to download & build Lua-Yajl yourself, see test/README.md"
	@echo "	  bench5a"
	@echo "		Crash test, Fleece only, same benchmarks but much longer running."
	@echo "		bench5 also include JSON4 and LuaJSON, it's for comparison, but less relevant"
	@echo "		bench5b runs ONLY JSON4 and LuaJSON, not Fleece, to pinpoint causes of possible crashes"
	@echo "	  bench6a"
	@echo "		obsolete"
	@echo "	  bench7"
	@echo "		performance comparison of escape modes, vs JSON4 and LuaJSON. This lists all escape"
	@echo "		settings and loops through a declining frequency of control characters"
	@echo "		if you are getting 'sample too small, could not measure', use bench7-1k"
	@echo "		for speedier testing, bench7a tests only Fleece, bench7c only Fleece and LuaJSON"

	@sleep 1
	@echo 
	@echo "Other Options"
	@echo "-------------"
	@echo 
	@echo "	  html"
	@echo "		create doc .html from .md, among other uses, for github gh-pages"
	@echo "	  delbak"
	@echo "		delete *.bak files"
	@echo "	  clean"
	@echo "		delete built files"

# make may get confused with test/ and INSTALL in a case-insensitive OS
dummy:

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all $(PLATS) clean test none dummy echo

# ----------------------------------------------------------------------
# Makefile fleece Mar 2011 - powered by Eonblast http://www.eonblast.com
