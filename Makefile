###-------------------------------------------------------------------------###
### Package     : Fleece - fast Lua to JSON conversion                      ###
### File        : ./Makefile                                                ###
### Description : Main make file, building, tests, benchmarks               ###
### Version     : 0.2.3 / alpha                                             ###
### Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ###
### Author      : H. Diedrich <hd2010@eonblast.com>                         ###
### License     : see file LICENSE                                          ###
### Created     :    Feb 2011                                               ###
### Changed     : 02 Mar 2011                                               ###
###-------------------------------------------------------------------------###
###                                                                         ###
###  Fleece is optimized for the fastest Lua to JSON conversion and beats   ###
###  other JSON implementations by around 10 times, native Lua up to 100x.  ###
###                                                                         ###
###-------------------------------------------------------------------------###
###                                                                         ###
###  Also see src/Makefile, src/fleece-intern.h and etc/luajson/Makefile.   ###
###                                                                         ###
###  PLEASE CONTRIBUTE YOUR LINUX, WINDOWS OR OTHER PLATFORM'S LINES        ###
###  FOR src/Makefile and etc/luajson/Makefile. Thank you.                  ###
###                                                                         ###
###-------------------------------------------------------------------------###
###                                                                         ###
###  Build: make <PLATFORM>                                                 ###
###  Test:  make test                                                       ###
###  Bench: make bench                                                      ###
###                                                                         ###
###  Run 'make' without arguments for more info and a list of platforms.    ###
###                                                                         ###
###-------------------------------------------------------------------------###

# Your platform. See PLATS for possible values.
PLAT= none

all: $(PLAT)

# use one of these, e.g. 'make linux'
PLATS= aix ansi bsd freebsd generic linux macosx macosx-old mingw posix solaris

# use one of these to build luajson for comparison
TPLATS= aix-test ansi-test bsd-test freebsd-test generic-test linux-test macosx-test macosx-old-test mingw-test posix-test solaris-test

# branch off to src/Makefile
$(PLATS):
	(cd src && $(MAKE) $@)

# make test comparison module luajson (not part of fleece)
$(TPLATS):
	(cd etc/luajson && $(MAKE) $@)

# simple tests
test:
	lua test/test-fleece-hello.lua
	lua test/test-fleece-json.lua
	lua test/test-fleece-size.lua

test2:
	lua test/test-fleece-rand.lua

test-test:
	lua test/test-json4.lua
	lua test/test-luajson.lua
	# lua test/test-yajl.lua

bench:
	lua test/bench-fleece.lua
	@echo Also see test/README.md

bench2:
	lua test/bench2.lua
	@echo Also see test/README.md

bench3:
	lua test/bench3.lua
	@echo Also see test/README.md

bench4:
	lua test/bench4.lua
	@echo Also see test/README.md

bench5:
	lua test/bench5.lua
	@echo Also see test/README.md

unit-tests:
	@echo "======================="
	@echo "Fleece Basic Unit Tests"
	@echo "======================="
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/basic.lua
	@echo "========================="
	@echo "Fleece Copmlex Unit Tests"
	@echo "========================="
	etc/lunit/lunit --path "etc/lunit/?.lua;test/unit/?.lua" test/unit/complex.lua

upversion:
	sed -E -e s/0\.2\.2/0.2.3/g -i .bak test/*
	sed -E -e s/0\.2\.2/0.2.3/g -i .bak test/unit/*
	sed -E -e s/0\.2\.2/0.2.3/g -i .bak src/*
	sed -E -e s/0\.2\.2/0.2.3/g -i .bak etc/*
	sed -E -e s/0\.2\.2/0.2.3/g -i .bak include/*	

delbak:
	rm -f *.bak
	rm -f */*.bak
	rm -f */*/*.bak

clean:
	cd src && $(MAKE) $@
	cd etc/luajson && $(MAKE) $@

html:
	lua etc/markdown.lua README.md
	lua etc/markdown.lua src/README.md
	lua etc/markdown.lua etc/README.md
	lua etc/markdown.lua test/README.md

none:
	@echo "To build Fleece, please do"
	@echo "   make <PLATFORM>"
	@echo "where <PLATFORM> is one of these:"
	@echo "   $(PLATS)"
	@echo "		e.g. 'make macosx'"
	@echo "		'macosx-old' is for Mac OS X < 10.4 / PPC, see http://lua-users.org/wiki/BuildingModules"
	@echo "		ONLY MACOSX and LINUX so far, please contribute your compilation commands. Thank you!"
	@echo "Other options:"
	@echo "   $(TPLATS)"
	@echo "		these make the luajson C lib, which is not part of fleece, for comparison"
	@echo "		ONLY MACOSX and LINUX so far, please contribute. Thank you!"
	@echo "   test"
	@echo "		runs minimal test scripts that check the health of the fleece build"
	@echo "   test2"
	@echo "		runs some randomly nested test data that demonstrates fleece' output"
	@echo "   test-test"
	@echo "		runs minimal test scripts that check the health of json4 and the luajson test build"
	@echo "   bench bench2 bench3 bench4 bench5"
	@echo "		runs bench mark scripts that compare fleece to json4, luajson, lua-yajl"
	@echo "   clean"
	@echo "		delete built files"

# make may get confused with test/ and INSTALL in a case-insensitive OS
dummy:

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all $(PLATS) clean test none dummy echo

# ----------------------------------------------------------------------
# Makefile fleece Mar 2011 - powered by Eonblast http://www.eonblast.com
