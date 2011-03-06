 /***-------------------------------------------------------------------------***\
| *** Packages    : Fleece - fast Lua to JSON module                          *** |
| *** File        : fleece-intern.h                                           *** |
| *** Description : internal header file                                      *** |
| *** Version     : 0.2.4 / alpha                                             *** |
| *** Requirement : self sufficient ANSI C                                    *** |
| *** Copyright   : (c) 2011 Eonblast Corporation http://www.eonblast.com     *** |
| *** Author      : H. Diedrich <hd2010@eonblast.com>                         *** |
| *** License     : see file LICENSE                                          *** |
| *** Created     : Feb 2011                                                  *** |
| *** Changed     : Feb 2011                                                  *** |
| ***-------------------------------------------------------------------------*** |
| ***                                                                         *** |
| ***  Untiday, most of the parameter related definitions are not used yet.   *** |
| ***  Most relevant are FLEECE_DEFAULT_LUCKY_BUF and                         *** |
| ***  FLEECE_DEFAULT_PARTS_BUF                                               *** |
| ***                                                                         *** |
| ***  Header files not cleaned up.                                           *** |
| ***                                                                         *** |
| ***-------------------------------------------------------------------------*** |
 \______________________________________________________________________________*/

#ifndef FLEECE_INTERN_H
#define FLEECE_INTERN_H

#include "fleece-config.h"

#ifdef FLEECE_WITH_ASSERTS
	#define assert_hard(cond, say) { if(!(cond))) { puts("\nfleece assertion failed: "); puts(say); puts("\n"); exit(0); } }
#else
	#define assert_hard(cond, say)
#endif

#define max(a,b) (a>=b?a:b)
#define min(a,b) (a<=b?a:b)


#include <string.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "fleece-buffers.h"
#include "fleece-insp_ctrl.h"


#define FLEECE_INCURSION_MAX_DEPTH  		1000 	/* make parameter */

#define FLEECE_BUFFER_STRATEGY       		1 		/* always succeeds when max_waste -1, else can fail */
#define FLEECE_PRECISE_STRATEGY      		2 		/* always succeeds */
#define FLEECE_PRECOUNT_STRATEGY     		3 		/* always succeeds */

#define FLEECE_DEFAULT_PRIMARY         		FLEECE_BUFFER_STRATEGY 
#define FLEECE_DEFAULT_SECONDARY       		FLEECE_PRECOUNT_STRATEGY

#define FLEECE_DEFAULT_MAX_DEPTH 			100 	/* allowed ... */
#define FLEECE_DEFAULT_MAX_WASTE 			20000 	/* allowed wasted space before switching to stragegy #2 */
// #define FLEECE_DEFAULT_LUCKY_BUF 			1000 	/* allowed buffer for first part of strategy #1 */
// #define FLEECE_DEFAULT_PARTS_BUF  			100 	/* allowed buffer for following parts in strategy #1 */
#define FLEECE_DEFAULT_MAX_BUF 			    1000000	/* allowed ... */
#define FLEECE_DEFAULT_MAX_HOLES 			1000 	/* allowed ... */

#define FLEECE_HASTY_FLOAT_SIZE     		32
#define FLEECE_HEX_FLOAT_SIZE     			2 *(sizeof(double)) + 1
#define FLEECE_RAW_FLOAT_SIZE     			sizeof(double) + 1
#define FLEECE_HASTY_INT_SIZE       		((size_t)(log10 (MAX_INT) +3)) /* use static local */

/* (+ctrl-flags+) */

#define FLEECE_WRONG_KEY_APPEND 			1
#define FLEECE_WRONG_KEY_IGNORE 			2 
#define FLEECE_WRONG_KEY_DISALLOW 			3

#define FLEECE_ARRAY_DECISION_LENGTH 		1
#define FLEECE_ARRAY_DECISION_SPEED 		2
#define FLEECE_ARRAY_DECISION_ALWAYS		3

#define FLEECE_QUIT_CONVERSION 				1
#define FLEECE_QUIT_NEVER 					2
#define FLEECE_QUIT_EXIT 					3

#define FLEECE_DEFAULT_ALLOW_ARRAYS         1
#define FLEECE_DEFAULT_SELF_CHECKS          0
#define FLEECE_DEFAULT_WRONG_KEY_HANDLING   FLEECE_WRONG_KEY_IGNORE
#define FLEECE_DEFAULT_ALLOW_HOLES          0
#define FLEECE_DEFAULT_INTEGER_CONVERSION   1
#define FLEECE_DEFAULT_ARRAY_DECISION       FLEECE_ARRAY_DECISION_SPEED
#define FLEECE_DEFAULT_FLOAT_FLAG          'g' /* f: fixed precision, g: brief, e: scientific exponents */
#define FLEECE_DEFAULT_PRECISION     		7 /* 0 = 7, > 22 = 22; but may be shorter for 'g' */
#define FLEECE_DEFAULT_WRITE_BOOL    		1 /* b: write 'true' and 'false', else 1 and 0 */
#define FLEECE_DEFAULT_USE_SPRINTF   		0 /* s: use sprintf() to make floats */
#define FLEECE_DEFAULT_HASTY_FLOAT   		1 /* h: hasty float count as 32 chars in pre-count */
#define FLEECE_DEFAULT_HASTY_INT    		0 /* i: hasty int count as 11 or 20 chars in pre-count */
#define FLEECE_DEFAULT_VERBOSE      		0 /* v: verbose */
#define FLEECE_DEFAULT_DEBUG         		0 /* d: debug extra verbose */
#define FLEECE_DEFAULT_USE_NULL             0
#define FLEECE_DEFAULT_MULTI_STRUCTURE      0
#define FLEECE_DEFAULT_ALLOW_OBJECTS        1
#define FLEECE_DEFAULT_POINTER_NOTATION     1
#define FLEECE_DEFAULT_QUIT_RULE            FLEECE_QUIT_EXIT
#define FLEECE_DEFAULT_RECURSE				1
#define FLEECE_DEFAULT_WRONG_TYPES          1
#define FLEECE_ARRAY_UNSORTED               0
#define FLEECE_DEFAULT_ARRAY_ZIP            0
#define FLEECE_DEFAULT_FUNC_STRING          "\"function(){}\""

/* performance compile switches */
#define FLEECE_WITH_VERBOSE     			 /* performance compile switch for no verbosity */
#define FLEECE_WITH_DEBUG       			 /* performance compile switch for no debug verbosity */
#define FLEECE_WITH_ASSERTS     			  1 /* performance compile switch to disable assertions */

void getsize(const TValue *o, insp_ctrl *ctrl);

insp_ctrl *insp_ctrl_malloc(const TValue *subj, char *flags, int precision, char **repr, int primary, 
		int secondary, size_t max_depth, size_t max_waste, size_t lucky_buf, 
		size_t parts_buf, size_t max_buf, size_t max_holes);

void free_insp_ctrl(insp_ctrl *ctrl);

#define DEB(_x) (if(ctrl->debug) { printf(_x); })

#define FLEECE_MAX_PREC 23

// safer
// #define FLEECE_ALWAYS_DELIMIT 1

#define LIT_NULL      "null"
#define LIT_TRUE      "true"
#define LIT_FALSE     "false"
#define LIT_FUNCTION  "[function]"
#define LIT_USER_DATA "[user data]"
#define LIT_THREAD    "[thread]"
#define LIT_TYPELESS  "[typeless data]"
#define LIT_LIGHT_USER_DATA "[light user data]"

/*
** max size of array part is 2^MAXBITS (excerpt from Lua ltable.c)
*/
#if LUAI_BITSINT > 26
#define MAXBITS		26
#else
#define MAXBITS		(LUAI_BITSINT-2)
#endif

static const char ESC=27;
static const char OPN=28;
static const char CLS=29;

#define true 1
#define false 0

size_t ilen(int value);
void dtoa (double x, char *str, int prec, char format, size_t *rlen);
size_t dlen (double x, int prec, char format);
size_t fdlen(double x, int prec, char format);
void dump_ctrl(insp_ctrl *ctrl);

#endif
