/**-------------------------------------------------------------------------***
*** Packages    : Fleece - fast Lua to JSON module                          ***
*** File        : fleece.h                                                  ***
*** Description : main outward header file - of not much use really as is   ***
*** Version     : 0.3.0 / alpha                                             ***
*** Requirement : Lua 5.1.4 - 5.1.4-2                                       ***
*** Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :    Feb 2011                                               ***
*** Changed     :    Feb 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Fleece is optimized for the fastest Lua to JSON conversion and beats   ***
***  other JSON implementations by around 10 times, native Lua up to 100x.  ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Header files not cleaned up.                                           ***
***                                                                         ***
***-------------------------------------------------------------------------**/

#include "lua.h"

#define LUA_FLEECE "fleece"

LUALIB_API int (luaopen_fleece) (lua_State *L);

