###-------------------------------------------------------------------------###
### Package     : Fleece - fast Lua to JSON module                          ###
### File        : etc/luajson/Makefile                                      ###
### Description : Building luajson as comparison package                    ###
### Author      : H. Diedrich <hd2010@eonblast.com>                         ###
### Created     :    Feb 2011                                               ###
### Changed     : 06 Mar 2011                                               ###
###-------------------------------------------------------------------------###
###                                                                         ###
###  see src/Makefile, src/fleece-intern.h and etc/luajson/Makefile         ###
###                                                                         ###
###  This is not actually part of Fleece.                                   ###
###                                                                         ###
###  PLEASE CONTRIBUTE YOUR *NIX, WINDOWS OR OTHER PLATFORM'S INSTRUCTIONS  ###
###  for src/Makefile and etc/luajson/Makefile. Thank you.                  ###
###                                                                         ###
###-------------------------------------------------------------------------###

# for comparison:
# makefile for building proton's luajson as dynamic C library
# (this is not fleece, but a different package, for comparison)

default: $(TPLAT)
 
clean:
	rm -f *. *.o *.a *.so

# convenience targets for popular platforms

none:
	@echo "Please choose a platform:"
	@echo "   $(TPLATS)"

aix-test:
	@echo "Not tested, please contribute"

ansi-test:
	@echo "Not tested, please contribute"

bsd-test:
	@echo "Not tested, please contribute"

freebsd-test:
	@echo "Not tested, please contribute"

generic-test:
	@echo "Not tested, please contribute"

linux-test:
	gcc -O2 -fomit-frame-pointer -c -o luajson.o luajson.c
	gcc -O -shared -fomit-frame-pointer -o luajson.so luajson.o

linux-64-test:
	gcc -O2 -fpic -c -o luajson.o luajson.c
	gcc -O -shared -fpic -o luajson.so luajson.o

macosx-test:
	gcc -bundle -undefined dynamic_lookup -o luajson.so luajson.c

macosx-old-test:
	export MACOSX_DEPLOYMENT_TARGET="10.3" && gcc -bundle -undefined dynamic_lookup -o luajson.so luajson.c

mingw-test:
	@echo "Not tested, please contribute"

posix-test:
	@echo "Not tested, please contribute"

solaris-test:
	@echo "Not tested, please contribute"

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all $(PLATS) default o a clean depend echo none

luajson.o: luajson.c luajson.h 

# ----------------------------------------------------------------------------------
# Makefile fleece/etc/luajson Feb 2011 - powered by Eonblast http://www.eonblast.com

