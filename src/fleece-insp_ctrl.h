/**-------------------------------------------------------------------------***
*** Packages    : Fleece - fast Lua to JSON module                          ***
*** File        : fleece-insp_ctrl.h                                        ***
*** Description : Lua data traversal and JSON str build control structure   ***
*** Version     : 0.2.4 / alpha                                             ***
*** Requirement : self sufficient ANSI C                                    ***
*** Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :    Feb 2011                                               ***
*** Changed     :    Feb 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Most of the parameters are not used yet.                               ***
***                                                                         ***
***-------------------------------------------------------------------------**/

#ifndef INSP_CTRL_H
#define INSP_CTRL_H 1

#include "fleece-config.h"
#include "fleece-intern.h"

/* control structure for traversal of data, both stringification and pre-count */
struct insp_ctrl_struct {

	/* parameters */

	const TValue *subject;	/* */
	char *flags;      		/* (+ctrl-flags+) [f|g|e|x|r] + [b|B] + [t|T] + [k|K] + [s|S] + [h|H] + [i|I] + [j|J] + [q|Q] + [v|V] + [d|D] */
	char **repr;      		/* ... */
	int precision;    		/* 0 = 7, > 22 = 22 */
	int primary;       		/* 0: default, 1: buffered, 2: pre-count, 3: precise parts */
	int secondary;    		/* 0: default, 1: buffered, 2: pre-count, 3: precise parts */
	size_t max_depth; 		/* allowed ... */
	size_t max_waste; 		/* allowed wasted space before switching to stragegy #3 */
	size_t lucky_buf; 		/* allowed buffer for trying strategy #1, the lucky shot */
	size_t parts_buf; 		/* allowed buffer for strategy #2, pre-allocated parts */
	size_t max_buf; 		/* allowed ... */
	size_t max_holes; 		/* allowed ... */
	char *func_str;   		/* string to use to represent function pointers */

	/* (+ctrl-flags+) */	
	char floatflag;   		/* f: fixed prec, g: brief, e: scient, x: hex bytes, r: raw bytes */
	int write_bool;   		/* b: write 'true' and 'false', else 1 and 0 */
	int use_sprintf;  		/* s: use sprintf() to make floats */
	int hasty_float;  		/* h: hasty float count as 32 chars in pre-count */
	int hasty_int;    		/* i: hasty int count as 11 or 20 chars in pre-count */
	int verbose;       		/* v: verbose */
	int debug;         		/* d: debug verbose */
	int allow_arrays;
	int self_checks;
	int wrong_key_handling;
	int allow_holes;
	int multi_structure;
	int pointer_notation;
	int quit_rule;
	int array_unsorted;
	int integer_conversion;
	int array_decision;
	int use_null;
	int wrong_types;
	int array_zip;
	int recurse;

	/* traversion control */
	size_t total_len;   	/* running counter of needed byte size */
	size_t total_parts; 	/* running counter of use parts buffers */
	size_t had_parts; 	    /* saved counter of use parts buffers */
	size_t depth;       	/* running counter of table recursion depth vs dead locks */
	size_t total_waste; 	/* running counter of unused allocated parts' buffers */
	size_t max_size;    	/* running counter of biggest met value buffer size */
	size_t lucky_time;  	/* running counter of processor time used for first part */
	size_t count_time;  	/* running counter of processor time used for pre-count */
	size_t coll_time;   	/* running counter of processor time used for collapse */
	size_t total_time;  	/* running counter of processor time used in total */

	/* strategy control */
	int strategy;       	/* current strategy - 1: buffer, 2: pre-count, 3: precise */
	int tried_sg1;      	/* strategy #1 has been tried and abandoned */
	char *error;        	/* user error message */
	char *severe;       	/* dev error message */
	int final_copy;       	/* there was a final copy for collapsing */
	
	/* parts_list */
	// string_parts 
	string_part *parts_list_first;
	string_part *parts_list_last;
	
	/* helper, since at least hasty_int_size is a formula */
	int hasty_int_size;
	int hasty_float_size;
};

typedef struct insp_ctrl_struct insp_ctrl;

#endif
