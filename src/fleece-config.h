/**-------------------------------------------------------------------------***
*** Packages    : Fleece - fast Lua to JSON module                          ***
*** File        : fleece-config.h                                           ***
*** Description : header with compile switches and default definitions      ***
*** Version     : 0.3.0 / alpha                                             ***
*** Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :  5 Mar 2011                                               ***
*** Changed     : 19 Mar 2011                                               ***
***-------------------------------------------------------------------------**/

#ifndef FLEECE_CONFIG_H
#define FLEECE_CONFIG_H 

/*****************************************************************************\
***                                                                         ***
***                             DEBUG SETTINGS                              ***
***                                                                         ***
\*****************************************************************************/
 
/* For debugging. Levels 0 to 10. 
 * 0 = none = fastest. 10 = all = slowest, very verbose.
 *  TODO: currently no good - won't compile */
#define SELFCHECK 0 
#define VERBOSITY 0

/* Safer mode where 0 is placed after each string as soon as it is written
 * into the buffer. Obviously, it's constantly overwritten right away. 
 * Uncomment to activate. */

// #define FLEECE_ALWAYS_DELIMIT

/*****************************************************************************\
***                                                                         ***
***                                PERFORMANCE                              ***
***                                                                         ***
\*****************************************************************************/

/** use assembler copy routines (x86 only) 
 *  Use this with make RATHER than uncommenting below:
 *  linux-asm, or linux-64-asm, or macosx-asm 
 *  which trigger this compile flag implicitly */
 
// #define FLEECE_ASM	

/* Allowed buffer byte size for first buffer. Affects the performance a lot. */

#define FLEECE_DEFAULT_LUCKY_BUF            100000

/* Allowed buffer size for following parts. 
 * Usually smaller, to reduce footprint. */

#define FLEECE_DEFAULT_PARTS_BUF            10000

/* shaky, better don't touch. The higher the faster with escapes. Must be > 2 
   !!! RSRV_BUF MUST BE SAME AS RESERVE_BUFFER, AS STRING !!! */

#define RESERVE_BUFFER 12
#define RSRV_BUF "12" 

/** Relevant for FLEECE_ASM: use string copy rather than memcpy, which is faster.
 *  The trade off is that Lua can have zeroes in strings, and this forfeits
 *  them, stopping traversion as soon as a 0 is met, and taking that as the end.
 *  This is faster than a memcpy, because the counter can be dropped. */
 
#define FLEECE_ZERO_AS_END             

/*****************************************************************************\
***                                                                         ***
***                                  FEATURES                               ***
***                                                                         ***
\*****************************************************************************/

/* compile with escape capability, else E, E1-4 are disabled.
 * not tested with 0 */
 
#define FLEECE_WITH_ESCAPE                  1   

/*****************************************************************************\
***                                                                         ***
***                                 DEFAULTS                                ***
***                                                                         ***
\*****************************************************************************/

/* which special characters to escape: 
 * E = E4; E0 none; E1 purge, E2 ",\; E3 ",\,/; E4 0-31,127,",\,/ */
 
#define FLEECE_DEFAULT_ESCAPE_FLAG          0   

/* TODO:Not currently implemented correctly. 
 * character to be used under E1 
 * to replace " and / */
#define FLEECE_PURGE_CHAR                   '*' 

#endif

