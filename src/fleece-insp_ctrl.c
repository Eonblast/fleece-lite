/**-------------------------------------------------------------------------***
*** Packages    : Fleece - fast Lua to JSON module                          ***
*** File        : fleece-insp_ctrl.c                                        ***
*** Description : Lua data traversal & JSON string build control structure  ***
*** Version     : 0.2.3 / alpha                                             ***
*** Requirement : self sufficient ANSI C                                    ***
*** Copyright   : (c) 2011 Eonblast Corporation                             ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :    Feb 2011                                               ***
*** Changed     :    Feb 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Most of the parameters are not used yet.                               ***
***                                                                         ***
***-------------------------------------------------------------------------**/

/* returns 1 when true-flag is found, 0 when false-flag, else default value  */
#define PARSE_FLAG(_feed, _t, _f, _def) \
	((!_feed) ? (_def) : strchr((_feed), (_t)) ? 1 : strchr((_feed), (_f)) ? 0 : (_def))

/* returns 1 when true-flag is found, 0 when false-flag, else default value  */
#define PARSE_TRI_FLAG(_feed, _1, _v1, _2, _v2, _3, _v3, _def) \
	((!_feed) ? (_def) : strchr((_feed), (_1)) ? (_v1) : strchr((_feed), (_2)) ? (_v2) : strchr((_feed), (_3)) ? (_v3) : (_def))

#define DISPLAY_TRI_FLAG(_n, _n1, _sym1, _n2, _sym2, _n3, _sym3, _def) \
	((_n)==(_n1)?(_sym1):(_n)==(_n2)?(_sym2):(_n)==(_n3)?(_sym3):(_def))

#define  DISPLAY_WRONG_KEY_FLAG(__n) \
	DISPLAY_TRI_FLAG(__n, \
	 		FLEECE_WRONG_KEY_APPEND, "E/append", \
	 		FLEECE_WRONG_KEY_IGNORE, "k/ignore", \
	 		FLEECE_WRONG_KEY_DISALLOW, "K/disallow", \
	 		"?")

/* create a inspection traversal control structure */
insp_ctrl *insp_ctrl_set(insp_ctrl *ctrl, const TValue *subj, char *flags, int precision, char **repr, int primary, 
		int secondary, size_t max_depth, size_t max_waste, size_t lucky_buf, 
		size_t parts_buf, size_t max_buf, size_t max_holes)
{
	//insp_ctrl *ctrl = malloc(sizeof(insp_ctrl));

	/* ------------------------------------------------------------- */
	/* parameters and defaults */
	/* ------------------------------------------------------------- */

	/* subject data (the table usually) */
	ctrl->subject = subj;

	/* FLAGS - default: 'abCDgHijkLMNopqrSTUVyZ' */ /* (+ctrl-flags+): */
	ctrl->flags = flags; 

	/** a: make arrays, see 'l' and 'W', shorter 
	 *  A: faster, never 
	 *  default: on (=when sensible)
	 */	
	 ctrl->allow_arrays   = PARSE_FLAG(flags, 'a', 'A', FLEECE_DEFAULT_ALLOW_ARRAYS);

	/** b: bools as 'true', 'false' 
	 *  B: 0, 1, shorter 
	 *  default: on (='true','false').  
	 */
	 ctrl->write_bool   = PARSE_FLAG(flags, 'b', 'B', FLEECE_DEFAULT_WRITE_BOOL);

	/** c: self checks 
	 *  C: way faster, off 
	 *  default: off.  
	 */
	 ctrl->self_checks   = PARSE_FLAG(flags, 'c', 'C', FLEECE_DEFAULT_SELF_CHECKS);

	/** d: debug output and checks 
	 *  D: way, way faster, off 
	 *  default: off.  
	 */
	 ctrl->debug  = PARSE_FLAG(flags, 'd', 'D', FLEECE_DEFAULT_DEBUG);

	/** E: append non-int, <1 keys at the end 
	 ** k: ignore values at non-int, <1 keys for arrays 
	 *  K: disallow 
	 *  default: k (=ignore values).  
	 */ 
	 ctrl->wrong_key_handling  = 
	 		PARSE_TRI_FLAG(flags, 
	 		'E', FLEECE_WRONG_KEY_APPEND,
	 		'k', FLEECE_WRONG_KEY_IGNORE,
	 		'K', FLEECE_WRONG_KEY_DISALLOW,
	 		FLEECE_DEFAULT_WRONG_KEY_HANDLING);

	/** e: scientific representation of floats ('n.nn...Enn') 
	 ** f: fixed precision representation of floats 
	 ** g: flexible representation of floats 
	 ** w: raw binary byte floats, fast but not JSON compatible 
	 ** x: hex binary byte floats 
	 *  default: 'g'.  
	 */
	 ctrl->floatflag    =
		! flags ?
		FLEECE_DEFAULT_FLOAT_FLAG:
		strchr(flags, 'e') ? 'e' :
		strchr(flags, 'f') ? 'f' :
		strchr(flags, 'g') ? 'g' :
		strchr(flags, 'w') ? 'w' :
		strchr(flags, 'x') ? 'x' :
		FLEECE_DEFAULT_FLOAT_FLAG;	 
	 
	/** h: use null to represent holes in arrays 
	 *  H: disallow holes 
	 *  default: off (=don't null).  
	 */
	 ctrl->allow_holes  = PARSE_FLAG(flags, 'h', 'H', FLEECE_DEFAULT_ALLOW_HOLES);

	/** i: try converting numbers to integers, shorter, usually faster 
	 *  I: always floats 
	 *  default: on (=try).  
	 */
	 ctrl->integer_conversion  = PARSE_FLAG(flags, 'i', 'I', FLEECE_DEFAULT_INTEGER_CONVERSION);
	 
	/** j: hasty, fast size estimate for floats 
	 *  J: slower, less memory 
	 *  default: on (=hasty).  
	 */
	 ctrl->hasty_float  = PARSE_FLAG(flags, 'j', 'J', FLEECE_DEFAULT_HASTY_FLOAT);
	 
	/** l: make arrays if the output is shorter 
	 *  L: make arrays when it's faster 
	 ** W: write arrays whenever possible, slower,
	 *  shorter with 'H' or 'z', can get very long with 'h'. 
	 *  default L (=when faster).
	 */
	 ctrl->array_decision  = 
	 		PARSE_TRI_FLAG(flags, 
	 		'l', FLEECE_ARRAY_DECISION_LENGTH,
	 		'L', FLEECE_ARRAY_DECISION_SPEED,
	 		'W', FLEECE_ARRAY_DECISION_ALWAYS,
	 		FLEECE_DEFAULT_ARRAY_DECISION);

	/** m: multi-structured output. 
	 *  M: single table output 
	 *  default: off (=single table).
	 */
	 ctrl->multi_structure  = PARSE_FLAG(flags, 'm', 'M', FLEECE_DEFAULT_MULTI_STRUCTURE);
 
	/** n: treat nil as not set 
	 *  N: use special NIL value 
	 *  default: off (=special NIL).  
	 */
	 ctrl->use_null  = PARSE_FLAG(flags, 'n', 'N', FLEECE_DEFAULT_USE_NULL);

	/** o: allow tables to be converted to objects 
	 *  O: disallow 
	 *  default: on (=allow).
	 */
	 ctrl->pointer_notation  = PARSE_FLAG(flags, 'o', 'O', FLEECE_DEFAULT_ALLOW_OBJECTS);
	 
	/** p: allow pointer notation for funcs, tables, userdata, threads 
	 *  P: disallow 
	 *  default: on (=allow).  
	 */
	 ctrl->pointer_notation  = PARSE_FLAG(flags, 'p', 'P', FLEECE_DEFAULT_POINTER_NOTATION);
	
	/** q: quit conversion on disallowed situation, return nil 
	 *  Q: ignore and continue 
	 ** X: quit Lua on disallowed situation 
	 *  default: q (=quit only conversion).  
	 */
	 ctrl->quit_rule  = 
	 		PARSE_TRI_FLAG(flags, 
	 		'q', FLEECE_QUIT_CONVERSION,
	 		'Q', FLEECE_QUIT_NEVER,
	 		'X', FLEECE_QUIT_EXIT,
	 		FLEECE_DEFAULT_QUIT_RULE);
	 
	/** r: recurse into tables 
	 *  R don't 
	 *  default: on (=recurse).  
	 */
	 ctrl->recurse  = PARSE_FLAG(flags, 'r', 'R', FLEECE_DEFAULT_RECURSE);

	/** s: standard number (float, double) generation 
	 *  S: faster conversion 
	 *  default: off (=faster).  
	 */
	 ctrl->use_sprintf  = PARSE_FLAG(flags, 's', 'S', FLEECE_DEFAULT_USE_SPRINTF);
	
	/** t: error on disallowed value types 
	 *  T: silently ignore 
	 *  default: off (=ignore).  
	 */
	 ctrl->wrong_types = PARSE_FLAG(flags, 't', 'T', FLEECE_DEFAULT_WRONG_TYPES);

	/** u: write unsorted arrays, ignore keys, shorter and faster 
	 *  U: slower, use keys for order 
	 *  default: off (=use keys).  
	 */
	 ctrl->array_unsorted  = PARSE_FLAG(flags, 'u', 'U', FLEECE_ARRAY_UNSORTED);
	 
	/** v:verbose 
	 *  V: silent 
	 *  default: off (=silent).  
	 */
	 ctrl->verbose  = PARSE_FLAG(flags, 'v', 'V', FLEECE_DEFAULT_VERBOSE);
	 
	/** y: hasty, fast size estimate for ints 
	 *  Y: slower, less memory 
	 *  default: on (=hasty).  
	 */
	 ctrl->hasty_int  = PARSE_FLAG(flags, 'y', 'Y', FLEECE_DEFAULT_HASTY_INT);
	
	/** z: zip arrays, close holes by shifting values down 
	 *  Z: don't 
	 *  default: off (=don't zip). 
	 */
	 ctrl->array_zip  = PARSE_FLAG(flags, 'z', 'Z', FLEECE_DEFAULT_ARRAY_ZIP);

	/* --- end of flags --- */

	/* ------------------------------------------------------------- */

	/* Floating point precision 0 = 7, > 22 = 22 */
	ctrl->precision = precision ? precision : FLEECE_DEFAULT_PRECISION;     

	/* string to use to represent function pointers */
	ctrl->repr      = 0;   // TODO
	ctrl->func_str  = FLEECE_DEFAULT_FUNC_STRING;   

	/* strategies - 0: default, 1: buffered, 2: pre-count, 3: precise parts */
	ctrl->primary   = primary ? primary : FLEECE_DEFAULT_PRIMARY;       
	ctrl->secondary = secondary ? secondary : FLEECE_DEFAULT_SECONDARY;     

	/* allowed recursion depth, into tables. Used to break circular deps */
	ctrl->max_depth = max_depth ? max_depth : FLEECE_DEFAULT_MAX_DEPTH; 

	/* allowed wasted space before switching from sg #1 to #2, or drop haste for #3 */
	ctrl->max_waste = max_waste ? max_waste : FLEECE_DEFAULT_MAX_WASTE;  

	/* allowed buffer for first part of strategy #1, the lucky shot */
	ctrl->lucky_buf = lucky_buf ? lucky_buf : FLEECE_DEFAULT_LUCKY_BUF; 

	/* allowed buffer sizes for strategy #1, pre-allocated parts after first */
	ctrl->parts_buf = parts_buf ? parts_buf : FLEECE_DEFAULT_PARTS_BUF;  

	/* allowed buffer sizes for ... */
	ctrl->max_buf = max_buf ? max_buf : FLEECE_DEFAULT_MAX_BUF;  

	/* allowed number of holes in tables, to not die from super sparse arrays */
	ctrl->max_holes = max_holes ? max_holes : FLEECE_DEFAULT_MAX_HOLES;  

	/* ------------------------------------------------------------- */

	/* init helper */
	ctrl->hasty_int_size = FLEECE_HASTY_INT_SIZE;
	ctrl->hasty_float_size = FLEECE_HASTY_FLOAT_SIZE - 
		
		max(0, min(FLEECE_MAX_PREC, FLEECE_MAX_PREC - ctrl->precision));
		
	/* ------------------------------------------------------------- */

	/* traversion control */
	ctrl->total_len   = 0; /* running counter of needed byte size */
	ctrl->total_parts = 0; /* running counter of use parts buffers */
	ctrl->had_parts   = 0; /* saved counter of use parts buffers */
	ctrl->depth       = 0; /* running counter of table recursion depth vs dead locks */
	ctrl->total_waste = 0; /* running counter of unused allocated parts' buffers */
	ctrl->max_size    = 0; /* running counter of biggest met value buffer size */
	ctrl->lucky_time  = 0; /* running counter of processor time used for first part */
	ctrl->count_time  = 0; /* running counter of processor time used for pre-count */
	ctrl->coll_time   = 0; /* running counter of processor time used for collapse */
	ctrl->total_time  = 0; /* running counter of processor time used in total */

	/* ------------------------------------------------------------- */

	/* strategy control */
	ctrl->strategy    = ctrl->primary; /* current strategy - 1: buffer, 2: pre-count, 3: precise */
	ctrl->tried_sg1   = 0;       /* strategy #1 has been tried and abandoned */
	ctrl->error       = 0;       /* user error message */
	ctrl->severe      = 0;       /* dev error message */
	ctrl->final_copy  = -1;      /* there was a final copy for collapsing */
	
	/* ------------------------------------------------------------- */

	/* parts_list */
	first_part(ctrl);
	// sets ctrl->parts_list_last and 
	// ctrl->parts_list_first.
	
	/* ------------------------------------------------------------- */

	// dump_ctrl(ctrl);
	
	return ctrl;
}

void free_insp_ctrl(insp_ctrl *ctrl)
{
	if(!ctrl) return;
	free_parts(ctrl->parts_list_first); // 0 ok.
	free(ctrl);
}

void insp_ctrl_close(insp_ctrl *ctrl)
{
	free_parts(ctrl->parts_list_first); // 0 ok.
}

/**
 * dump the contents of the control structure
 */
void dump_ctrl(insp_ctrl *ctrl)
{
	puts("-------------------------------------\n");
	puts("Inspection Control State:\n");
	puts("(defaults in brackets.)\n");
	printf("size: %zd\n", sizeof(insp_ctrl));
	
	printf("primary strategy  : %d (%d) %s\n", ctrl->primary, FLEECE_DEFAULT_PRIMARY,
		(ctrl->primary == (FLEECE_DEFAULT_PRIMARY) ? "" : "*" ));

	printf("secondary strategy: %d (%d) %s\n", ctrl->secondary, FLEECE_DEFAULT_SECONDARY,
		(ctrl->secondary == (FLEECE_DEFAULT_SECONDARY) ? "" : "*" ));

	/* FLAGS - default: 'abCDgHijkLMNopqrSTUVyZ' */ /* (+ctrl-flags+): */
	printf("flags: %s (%s) %s\n", ctrl->flags ? ctrl->flags : "not set", "abCDgHijkLMNopqrSTUVyZ", ctrl->flags ? strcmp(ctrl->flags,"abCDgHijkLMNopqrSTUVyZ") ? "*" : "" : "");

	printf("flag a/A (allow arrays): %s (%s) %s\n", (ctrl->allow_arrays   ?"on":"off"),
		(ctrl->allow_arrays?"a":"A"), (ctrl->allow_arrays==FLEECE_DEFAULT_ALLOW_ARRAYS?"":"*"));

	printf("flag b/B (write bool): %s (%s) %s\n", (ctrl->write_bool ?"on":"off"),
		(ctrl->write_bool?"b":"B"), (ctrl->write_bool==FLEECE_DEFAULT_WRITE_BOOL?"":"*")); 

	printf("flag c/C (self checks): %s (%s) %s\n", (ctrl->self_checks?"on":"off"),
		(ctrl->self_checks?"c":"C"), (ctrl->self_checks==FLEECE_DEFAULT_SELF_CHECKS?"":"*")); 

	printf("flag d/D (debug): %s (%s) %s\n", (ctrl->debug  ?"on":"off"),
		(ctrl->debug  ?"d":"D"), (ctrl->debug   == FLEECE_DEFAULT_DEBUG ? "" : "*")); 

	printf("flag E/k/K (wrong key handling): %s[%d] (%s[%d]) %s\n", 
		DISPLAY_WRONG_KEY_FLAG(ctrl->wrong_key_handling), ctrl->wrong_key_handling,
		DISPLAY_WRONG_KEY_FLAG(FLEECE_DEFAULT_WRONG_KEY_HANDLING), FLEECE_DEFAULT_WRONG_KEY_HANDLING,
		(ctrl->wrong_key_handling==FLEECE_DEFAULT_WRONG_KEY_HANDLING?"":"*")); 

	printf("flag e/f/g/w/x (float flag): %c (%c) %s\n", ctrl->floatflag,
		FLEECE_DEFAULT_FLOAT_FLAG, 
		(ctrl->floatflag==FLEECE_DEFAULT_FLOAT_FLAG?"":"*")); 
	 
	printf("flag h/H (allow holes): %s (%s) %s\n", (ctrl->allow_holes  ?"on":"off"), (ctrl->allow_holes  ?"h":"H"), (ctrl->allow_holes   == FLEECE_DEFAULT_ALLOW_HOLES ? "" : "*")); 

	printf("flag i/I (integer conversion): %s (%s) %s\n", (ctrl->integer_conversion  ?"on":"off"), (ctrl->integer_conversion  ?"i":"I"), (ctrl->integer_conversion   == FLEECE_DEFAULT_INTEGER_CONVERSION ? "" : "*")); 
	 
	printf("flag j/J (hasty float): %s (%s) %s\n", (ctrl->hasty_float  ?"on":"off"), (ctrl->hasty_float  ?"j":"J"), (ctrl->hasty_float   == FLEECE_DEFAULT_HASTY_FLOAT ? "" : "*")); 
	 
	printf("flag l/L/W (array decision): %d (%d) %s\n", ctrl->array_decision,
		FLEECE_DEFAULT_ARRAY_DECISION,
		(ctrl->array_decision==FLEECE_DEFAULT_ARRAY_DECISION?"":"*")); 

	printf("flag m/M (multi structure): %s (%s) %s\n", (ctrl->multi_structure  ?"on":"off"), (ctrl->multi_structure  ?"m":"M"), (ctrl->multi_structure   == FLEECE_DEFAULT_MULTI_STRUCTURE ? "" : "*")); 
 
	printf("flag n/N (use null): %s (%s) %s\n", (ctrl->use_null  ?"on":"off"), (ctrl->use_null  ?"n":"N"), (ctrl->use_null   == FLEECE_DEFAULT_USE_NULL ? "" : "*")); 

	printf("flag o/O (allow objects): %s (%s) %s\n", (ctrl->pointer_notation  ?"on":"off"), (ctrl->pointer_notation  ?"o":"O"), (ctrl->pointer_notation   == FLEECE_DEFAULT_ALLOW_OBJECTS ? "" : "*")); 
	 
	printf("flag p/P (pointer notation): %s (%s) %s\n", (ctrl->pointer_notation  ?"on":"off"), (ctrl->pointer_notation  ?"p":"P"), (ctrl->pointer_notation   == FLEECE_DEFAULT_POINTER_NOTATION ? "" : "*")); 
	
	printf("flag q/Q/X (quit rule): %d (%d) %s\n", ctrl->quit_rule,
		FLEECE_DEFAULT_QUIT_RULE, 
		(ctrl->quit_rule==FLEECE_DEFAULT_QUIT_RULE?"":"*")); 
	 
	printf("flag r/R (recurse): %s (%s) %s\n", (ctrl->recurse  ?"on":"off"), (ctrl->recurse  ?"r":"R"), (ctrl->recurse   == FLEECE_DEFAULT_RECURSE ? "" : "*")); 

	printf("flag s/S (use sprintf): %s (%s) %s\n", (ctrl->use_sprintf  ?"on":"off"), (ctrl->use_sprintf  ?"s":"S"), (ctrl->use_sprintf   == FLEECE_DEFAULT_USE_SPRINTF ? "" : "*")); 
	
	printf("flag t/T (wrong types): %s (%s) %s\n", (ctrl->wrong_types ?"on":"off"), (ctrl->wrong_types ?"t":"T"), (ctrl->wrong_types  == FLEECE_DEFAULT_WRONG_TYPES ? "" : "*")); 

	printf("flag u/U (array unsorted): %s (%s) %s\n", (ctrl->array_unsorted ?"on":"off"),
		(ctrl->array_unsorted ?"u":"U"), (ctrl->array_unsorted  == FLEECE_ARRAY_UNSORTED ? "" : "*")); 
	 
	printf("flag v/V (verbose): %s (%s) %s\n", (ctrl->verbose  ?"on":"off"), (ctrl->verbose  ?"v":"V"), (ctrl->verbose   == FLEECE_DEFAULT_VERBOSE ? "" : "*")); 
	 
	printf("flag y/Y (hasty int): %s (%s) %s\n", (ctrl->hasty_int  ?"on":"off"), (ctrl->hasty_int  ?"y":"Y"), (ctrl->hasty_int == FLEECE_DEFAULT_HASTY_INT ? "" : "*")); 
	
	printf("flag z/Z (array zip): %s (%s) %s\n", (ctrl->array_zip  ?"on":"off"), (ctrl->array_zip  ?"z":"Z"), (ctrl->array_zip == FLEECE_DEFAULT_ARRAY_ZIP ? "" : "*")); 

	/* --- end of flags --- */
	printf("precision   : %d (%d) %s (Floating point precision 0 = 7, > 22 = 22)\n", ctrl->precision,
		FLEECE_DEFAULT_PRECISION, (ctrl->precision==FLEECE_DEFAULT_PRECISION?"":"*"));

	printf("lucky_buf   : %zd (%d) %s (allowed first buffer of strategy #1)\n", ctrl->lucky_buf,
		FLEECE_DEFAULT_LUCKY_BUF, (ctrl->lucky_buf==FLEECE_DEFAULT_LUCKY_BUF?"":"*"));

	printf("parts_buf   : %zd (%d) %s (allowed buffer sizes for sg #1 after first)\n", ctrl->parts_buf,
		FLEECE_DEFAULT_PARTS_BUF, (ctrl->parts_buf==FLEECE_DEFAULT_PARTS_BUF?"":"*"));

	printf("max_waste   : %zd (%d) %s (allowed wasted space before abandoning #1, or drop haste for #3)\n",
		ctrl->max_waste, FLEECE_DEFAULT_MAX_WASTE, (ctrl->max_waste==FLEECE_DEFAULT_MAX_WASTE?"":"*"));

	printf("func_str    : %s (%s) %s (string to use to represent function pointers)\n", ctrl->func_str,
		FLEECE_DEFAULT_FUNC_STRING, (ctrl->func_str==FLEECE_DEFAULT_FUNC_STRING?"":"*"));

	/* init helper */
	printf("hasty_int_size    : %d (calculated at compile time)\n", ctrl->hasty_int_size);
	printf("hasty_float_size  : %d (calculated at run time using precision)\n", ctrl->hasty_float_size);	

	/* traversion control */
	printf("total_len  : %5zd  (running counter of needed byte size)\n", ctrl->total_len);
	printf("total_parts: %5zd  (running counter of use parts buffers)\n", ctrl->total_parts );
	printf("depth      : %5zd  (running counter of table recursion depth)\n", ctrl->depth);
	printf("total_waste: %5zd  (running counter of unused allocated parts' buffers)\n", ctrl->total_waste);
	printf("max_size   : %5zd  (running counter of biggest met value buffer size)\n", ctrl->max_size);
	printf("lucky_time : %5zd  (running counter of processor time used for first part)\n", ctrl->lucky_time);
	printf("count_time : %5zd  (running counter of processor time used for pre-count)\n", ctrl->count_time);
	printf("coll_time  : %5zd  (running counter of processor time used for collapse)\n", ctrl->coll_time);
	printf("total_time : %5zd  (running counter of processor time used in total)\n", ctrl->total_time);
	
	/* strategy control */
	printf("strategy   : %d  (current strategy - 1: buffer, 2: pre-count, 3: precise)\n", ctrl->strategy);
	printf("tried_sg1  : %s  (strategy #1 has been tried and abandoned)\n", ctrl->tried_sg1 ? "yes" : "no");

	printf("error      : %s  (user error message)\n", ctrl->error);
	printf("severe     : %s  (dev error message)\n", ctrl->severe);	

	puts("-------------------------------------\n");
}
