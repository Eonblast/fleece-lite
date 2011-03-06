/**-------------------------------------------------------------------------***
*** Package     : Fleece Lite - Lua to JSON conversion                      ***
*** File        : fleece-lua.c                                              ***
*** Description : interface to to Lua                                       ***
*** Version     : 0.2.4 / alpha                                             ***
*** Requirement : Lua 5.1.4 - 5.1.4-2                                       ***
*** Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :    Feb 2011                                               ***
*** Changed     : 02 Mar 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Fleece is optimized for the fastest Lua to JSON conversion and beats   ***
***  other JSON implementations by around 10 times, native Lua up to 100x.  ***
***  This file contains not much interesting stuff in this regard. There    ***
***  is one bit original Lua source copied here to speed things up.         ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Header files not cleaned up.                                           ***
***                                                                         ***
***-------------------------------------------------------------------------**/

#include "fleece-config.h"

LUALIB_API int luaopen_fleece (lua_State *L);
static int fleece_size(lua_State *L);
static int fleece_json(lua_State *L);
static int fleece_hello(lua_State *L);

static int fleece_hello(lua_State *L) {
	printf("Hello LuaJSON World!\n");
	return 1;
}

static const struct luaL_Reg fleece [] = {
	{"hello", fleece_hello }, /* registering the function */
	{"size", fleece_size }, /* dry run to find the JSON result size, without creating it */
	{"json", fleece_json }, /* convert (stringify) a Lua value to a JSON string */
	{ NULL, NULL }
};

LUALIB_API int luaopen_fleece (lua_State *L) {
	luaL_register(L, "fleece", fleece); /* registering the module */
	return 1;
}

// --------------------------------------------------------------------
// UTIL
// --------------------------------------------------------------------

#define VERB(m) printf("fleece: %s\n", m)
static TValue *index2adr (lua_State *L, int idx); /// TODO: put that decl somewhere

int return_nil(lua_State *L)
{
    lua_settop(L, 0);   // Clear stack
    lua_pushnil(L);
    return 1;
}

/* PARAMETERS, LUA SIDE
 * 
 *  NON IMPLEMENTED YET BEYOND THE FIRST, THE TABLE.
 */
 // (max balanced) -12 +0
insp_ctrl *insp_ctrl_init(insp_ctrl *ctrl, lua_State *L)
{
    lua_settop(L, 12);   // arguments, max 12

	const TValue *subj  = index2adr(L, -12);
	char *flags         = (char *)lua_tostring(L, -11);
	int precision       = (int)lua_tointeger(L, -10);
//	const TValue *repr  = index2adr(L, -9);
	int primary         = (int)lua_tointeger(L, -8);
	int secondary       = (int)lua_tointeger(L, -7);
	size_t max_depth    = (size_t)lua_tointeger(L, -6);
	size_t max_waste    = (size_t)lua_tointeger(L, -5);
	size_t lucky_buf    = (size_t)lua_tointeger(L, -4);
	size_t parts_buf    = (size_t)lua_tointeger(L, -3);
	size_t max_buf      = (size_t)lua_tointeger(L, -2);
	size_t max_holes    = (size_t)lua_tointeger(L, -1);

	lua_pop(L, 12);

	insp_ctrl_set(ctrl, subj, flags, precision, 0, primary, secondary,
		max_depth, max_waste, lucky_buf, parts_buf, max_buf, max_holes);

    return ctrl;
}

/**
 * dry run to find the JSON result size, without creating it
 */
static int fleece_size(lua_State *L) {

	insp_ctrl ctrl;
	insp_ctrl_init(&ctrl, L);

	fleece_size_(&ctrl, ctrl.subject);
	size_t size = ctrl.total_len;
	lua_pushnumber(L, size);

	insp_ctrl_close(&ctrl);
	return 1;
}


/**
 * convert (stringify) a Lua value to a JSON string
 */
static int fleece_json(lua_State *L) {

	#if VERBOSITY >= 5
	printf("\n----------------------------------------------\n");
	#endif

	insp_ctrl ctrl;
	insp_ctrl_init(&ctrl, L);

	void *freer;
	char *str = fleece_stringify(&ctrl, ctrl.subject, &freer);
	lua_pushstring(L, str); // *)

	insp_ctrl_close(&ctrl); 
	free_collapsed(freer); // (keeps lucky)

	#if VERBOSITY >= 5
	printf("\n==============================================\n");
	#endif
	
	return 1;
	
	/* *) Pushes the zero-terminated string pointed to by s onto the
	stack. Lua makes (or reuses) an internal copy of the given
	string, so the memory at s can be freed or reused immediately
	after the function returns. The string cannot contain embedded
	zeros; it is assumed to end at the first zero. */
}

/* -------------------------------------------------------------------------- */

/* copy of original Lua source, which Lua does not export */
static TValue *index2adr (lua_State *L, int idx) {

  if (idx > 0) {   
    TValue *o = L->base + (idx - 1);
    api_check(L, idx <= L->ci->top - L->base);
    if (o >= L->top) return cast(TValue *, luaO_nilobject);
    else return o;  
  }
  else if (idx > LUA_REGISTRYINDEX) {
    api_check(L, idx != 0 && -idx <= L->top - L->base);
    return L->top + idx;
  }
  else switch (idx) {  /* pseudo-indices */
    case LUA_REGISTRYINDEX: return registry(L);
    case LUA_ENVIRONINDEX: {
      Closure *func = curr_func(L);
      sethvalue(L, &L->env, func->c.env);
      return &L->env;
    }
    case LUA_GLOBALSINDEX: return gt(L);
    default: {
      Closure *func = curr_func(L);
      idx = LUA_GLOBALSINDEX - idx;
      return (idx <= func->c.nupvalues)
                ? &func->c.upvalue[idx-1]
                : cast(TValue *, luaO_nilobject);
    }
  }
}
