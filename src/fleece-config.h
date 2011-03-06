/**-------------------------------------------------------------------------***
*** Packages    : Fleece - fast Lua to JSON module                          ***
*** File        : fleece-config.h                                           ***
*** Description : header with compile switches and default definitions      ***
*** Version     : 0.2.4 / alpha                                             ***
*** Requirement : self sufficient ANSI C                                    ***
*** Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     : 5 Mar 2011                                                ***
*** Changed     : 5 Mar 2011                                                ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Header files not cleaned up.                                           ***
***                                                                         ***
***-------------------------------------------------------------------------**/

#ifndef FLEECE_CONFIG_H
#define FLEECE_CONFIG_H 1

#define FLEECE_VERSION 0.2.4

/* levels 0 to 10. 0 = none = fastest */
#define SELFCHECK 0
#define VERBOSITY 0

#define FLEECE_DEFAULT_LUCKY_BUF 			10000 	/* allowed buffer for first part of strategy #1 */
#define FLEECE_DEFAULT_PARTS_BUF  			1000 	/* allowed buffer for following parts in strategy #1 */

#endif
