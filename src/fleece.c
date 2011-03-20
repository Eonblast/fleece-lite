/**-------------------------------------------------------------------------***
*** Package     : Fleece - fast Lua to JSON conversion                      ***
*** File        : fleece.c                                                  ***
*** Description : main source file: includes all others                     ***
*** Version     : 0.3.1 / alpha                                             ***
*** Requirement : Lua 5.1.4 - 5.1.4-2                                       ***
*** Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :    Feb 2011                                               ***
*** Changed     : 18 Mar 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Fleece is optimized for the fastest Lua to JSON conversion and beats   ***
***  other JSON implementations by around 10 times, native Lua up to 100x.  ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  This file includes all other src/fleece-*.c files.                     ***
***                                                                         ***
***  Header files are not cleaned up.                                       ***
***                                                                         ***
***-------------------------------------------------------------------------**/

/* C headers --------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

/* Lua headers (in include/) ----------------------------------------------- */
#include "lobject.h"
#include "ltable.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "lmem.h"
#include "lstate.h"

/* Fleece headers (in src/) ------------------------------------------------ */
#include "fleece-intern.h"
#include "fleece.h"

/* Fleece sources (in src/) ------------------------------------------------ */
#include "fleece-buffers.c"
#include "fleece-copy.c"
#include "fleece-ctrl.c"
#include "fleece-numbers.c"
#include "fleece-size.c"
#include "fleece-stringify.c"
#include "fleece-lua.c"

