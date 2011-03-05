###-------------------------------------------------------------------------###
### Package     : Fleece - fast Lua to JSON module                          ###
### File        : Makefile                                                  ###
### Description : makefile for building Fleece as dynamic library           ###
### Author      : H. Diedrich <hd2010@eonblast.com>                         ###
### Created     :    Feb 2011                                               ###
### Changed     : 02 Mar 2011                                               ###
###-------------------------------------------------------------------------###
###                                                                         ###
###  Don't use directly but use ../Makefile, see there or ../README.md      ###
###                                                                         ###
###  Also fleece-intern.h for compile time configurations.                  ###
###                                                                         ###
###  PLEASE CONTRIBUTE YOUR LINUX, WINDOWS OR OTHER PLATFORM'S LINES        ###
###  FOR src/Makefile and etc/luajson/Makefile. Thank you.                  ###
###                                                                         ###
###-------------------------------------------------------------------------###

default: $(PLAT)
 
clean:
	rm -f *. *.o *.a *.so

# convenience targets for popular platforms

none:
	@echo "Please choose a platform:"
	@echo "   $(PLATS)"

aix:
	@echo "Not tested, please contribute"

ansi:
	@echo "Not tested, please contribute"

bsd:
	@echo "Not tested, please contribute"

freebsd:
	@echo "Not tested, please contribute"

generic:
	@echo "Not tested, please contribute"

linux:
	gcc -O2 -fomit-frame-pointer -I../include -c -o fleece.o fleece.c
	gcc -O -shared -fomit-frame-pointer -o fleece.so fleece.o

macosx:
	gcc -bundle -undefined dynamic_lookup -I../include -o fleece.so fleece.c

macosx-old: 
	export MACOSX_DEPLOYMENT_TARGET="10.3" && gcc -bundle -undefined dynamic_lookup -I../include -o fleece.so fleece.c

mingw:
	@echo "Not tested, please contribute"

posix:
	@echo "Not tested, please contribute"

solaris:
	@echo "Not tested, please contribute"

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all $(PLATS) default o a clean depend echo none

luajson.o: luajson.c luajson.h 
fleece.o: fleece.h fleece-config.h fleece-intern.h fleece.c fleece-buffers.c \
	fleece-numbers.c fleece-insp_ctrl.c fleece-size.c fleece-stringify.c fleece-lua.c 

# --------------------------------------------------------------------------
# Makefile fleece/src Feb 2011 - powered by Eonblast http://www.eonblast.com
