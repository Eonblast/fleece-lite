/**-------------------------------------------------------------------------***
*** Packages    : Fleece - fast Lua to JSON module                          ***
*** File        : fleece-buffers.h                                          ***
*** Description : header, incl. main macros for custom string buffers       ***
*** Version     : 0.3.1 / alpha                                             ***
*** Requirement : self sufficient ANSI C                                    ***
*** Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :    Feb 2011                                               ***
*** Changed     : 18 Mar 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Header files not cleaned up.                                           ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Todo: try strcpy for non-asm                                           ***
***                                                                         ***
***-------------------------------------------------------------------------**/
// TODO: Try strcpy for non-asm.

#ifndef FLEECE_BUFFERS_H
#define FLEECE_BUFFERS_H 1

#include "fleece-config.h"
#include "fleece-intern.h"

/*****************************************************************************\
***                                                                         ***
***  COPYING MACROS                                                         ***
***                                                                         ***
\*****************************************************************************/

/*---------------------------------------------------------------------------*\
**  Memory Copy Implementation Choice                                        **
\*---------------------------------------------------------------------------*/

#ifndef FLEECE_ASM

    #define FLEECE_MEMCPY fast_memcpy_caster_16
    #define FLEECE_STRCPY fast_memcpy_caster_16
#else // ASM:
  #define FLEECE_MEMCPY memcpy
  #ifdef FLEECE_ZERO_AS_END
    #define FLEECE_STRCPY strcpy_word_blindly
  #else
    #define FLEECE_STRCPY fast_memcpy_caster_16
  #endif
#endif

/*****************************************************************************/

/*---------------------------------------------------------------------------*\
**  Buffer Control Structure                                                 **
\*---------------------------------------------------------------------------*/

/* collect string parts, dynamic, or fix.  Dynamic is stored post-adjacent. */
struct string_part_struct { 
	struct string_part_struct *next;
	char *buf; /* for fixed */
	size_t len;
	size_t bufsize;

};

#define is_fixed_buffer(part) ((part)->buf != 0)

/*---------------------------------------------------------------------------*\
**  Block Copy                                                               **
\*---------------------------------------------------------------------------*/

/* Structures to use for the caster copy routines below */
struct char2  { char mem[2]; };  typedef struct char2 *buf2;
struct char3  { char mem[3]; };  typedef struct char3 *buf3;
struct char4  { char mem[4]; };  typedef struct char4 *buf4;
struct char5  { char mem[5]; };  typedef struct char5 *buf5;
struct char6  { char mem[6]; };  typedef struct char6 *buf6;
struct char7  { char mem[7]; };  typedef struct char7 *buf7;
struct char8  { char mem[8]; };  typedef struct char8 *buf8;
struct char9  { char mem[9]; };  typedef struct char9 *buf9;
struct char10 { char mem[10]; }; typedef struct char10 *buf10;
struct char11 { char mem[11]; }; typedef struct char11 *buf11;
struct char12 { char mem[12]; }; typedef struct char12 *buf12;
struct char13 { char mem[13]; }; typedef struct char13 *buf13;
struct char14 { char mem[14]; }; typedef struct char14 *buf14;
struct char15 { char mem[15]; }; typedef struct char15 *buf15;
struct char16 { char mem[16]; }; typedef struct char16 *buf16;

/* copy up to 4 bytes as block (not used) */
#define fast_memcpy_caster_4(_dest, _src, _count) { \
\
	if(_count<=4) { \
		switch(_count) { \
		case 1:  *(char*)_dest = *(char*)_src; break; \
		case 2:  *(buf2)_dest  = *(buf2)_src;  break; \
		case 3:  *(buf3)_dest  = *(buf3)_src;  break; \
		case 4:  *(buf4)_dest  = *(buf4)_src;  break; \
		} \
	} \
	else { \
		memcpy(_dest, _src, _count); \
    } \
}

/* copy up to 8 bytes as block (not used) */
#define fast_memcpy_caster_8(_dest, _src, _count) { \
\
	if(_count<=8) { \
		switch(_count) { \
		case 1:  *(char*)_dest = *(char*)_src; break; \
		case 2:  *(buf2)_dest  = *(buf2)_src;  break; \
		case 3:  *(buf3)_dest  = *(buf3)_src;  break; \
		case 4:  *(buf4)_dest  = *(buf4)_src;  break; \
		case 5:  *(buf5)_dest  = *(buf5)_src;  break; \
		case 6:  *(buf6)_dest  = *(buf6)_src;  break; \
		case 7:  *(buf7)_dest  = *(buf7)_src;  break; \
		case 8:  *(buf8)_dest  = *(buf8)_src;  break; \
		} \
	} \
	else { \
		memcpy(_dest, _src, _count); \
    } \
}

/* copy up to 16 bytes as block */
#define fast_memcpy_caster_16(_dest, _src, _count) { \
\
	if(_count<=16) { \
		switch(_count) { \
		case 1:  *(char*)_dest = *(char*)_src; break; \
		case 2:  *(buf2)_dest  = *(buf2)_src;  break; \
		case 3:  *(buf3)_dest  = *(buf3)_src;  break; \
		case 4:  *(buf4)_dest  = *(buf4)_src;  break; \
		case 5:  *(buf5)_dest  = *(buf5)_src;  break; \
		case 6:  *(buf6)_dest  = *(buf6)_src;  break; \
		case 7:  *(buf7)_dest  = *(buf7)_src;  break; \
		case 8:  *(buf8)_dest  = *(buf8)_src;  break; \
		case 9:  *(buf9)_dest  = *(buf9)_src;  break; \
		case 10: *(buf10)_dest = *(buf10)_src; break; \
		case 11: *(buf11)_dest = *(buf11)_src; break; \
		case 12: *(buf12)_dest = *(buf12)_src; break; \
		case 13: *(buf13)_dest = *(buf13)_src; break; \
		case 14: *(buf14)_dest = *(buf14)_src; break; \
		case 15: *(buf15)_dest = *(buf15)_src; break; \
		case 16: *(buf16)_dest = *(buf16)_src; break; \
		} \
	} \
	else { \
		memcpy(_dest, _src, _count); \
    } \
}

#ifdef FLEECE_ASM
#define strcpy_word_blindly(dst, src, len) { \
\
    char *dummy0, *dummy1, *dummy2; \
    __asm__ __volatile__(  \
    					   "1:\tlodsw\n\t" \
    					   "cmp $0, %%al\n\t" \
    					   "je end%=\n\t" \
    					   "cmp $0, %%ah\n\t" \
    					   "je stutterend%=\n\t" \
	                       "stosw\n\t" \
                           "jmp 1b\n\t" \
                           "stutterend%=:" \
	                       "stosb\n\t" \
                           "end%=:" \
                         : "=&S" (dummy0), "=&D" (dummy1), "=&a" (dummy2) \
                         : "0" (src),"1" (dst) \
                         : "memory", "cc"); \
}
#endif

/* collect string parts, dynamic, or fix.  Dynamic is stored post-adjacent. */
typedef struct string_part_struct string_part;

#include "fleece-ctrl.h"

inline void first_part(insp_ctrl *ctrl);
inline void buffer_add(insp_ctrl *ctrl, size_t minlen);
inline void multi_add_dyn_part(string_part **last, 
	char pre1, char pre2, const char *add, char post1, char post2, size_t len, 
	size_t *total_len, size_t *total_parts, size_t *waste, size_t buf_size);
inline void add_dyn_part(string_part **last, const char *add, size_t len, size_t *total_len, 
	size_t *total_parts, size_t *waste, size_t buf_size);
inline void add_fix_part(string_part **last, const char const *add, size_t len, size_t *total_len,
	size_t *total_parts, size_t *waste);
inline char *collapse_parts(string_part **first, size_t total_len, size_t parts_control);
inline size_t free_parts(string_part *first);

#define dynbuf(part) ((char *)((part) + 1))
#define dynp(part) ((char *)((part) + 1) + (part)->len)
#define dynp_l(part, _len) ((char *)((part) + 1) + _len)
#define dynleft(part) ((part)->bufsize - (part)->len)

#ifdef FLEECE_ALWAYS_DELIMIT
#define DELIMIT(last) *dynp(last) = 0;
#else
#define DELIMIT(last) 
#endif

#if VERBOSITY >= 7
#define bufcpy(__ctrl, __add, __len) { \
		  string_part *last = __ctrl->parts_list_last; \
		  printf("fleece @7: adding string of length %zd\n", __len); \
	  	  FLEECE_STRCPY(dynp(last), __add, __len); \
	  	  last->len += __len; \
	  	  __ctrl->total_len += __len; \
		  printf("fleece @7: buffer %p buffer length now %zd, total length now %zd.\n", \
		  	last, last->len, __ctrl->total_len); \
	  	  DELIMIT(last) \
	    }
#else
#define bufcpy(___ctrl, ___add, ___len) { \
		  string_part *last = ___ctrl->parts_list_last; \
	  	  FLEECE_STRCPY(dynp(last), ___add, ___len); \
	  	  last->len += ___len; \
	  	  ___ctrl->total_len += ___len; \
	  	  DELIMIT(last) \
	  	}
#endif

#if VERBOSITY >= 7
#define bufcpy_escaped(__ctrl, __add, __len, __buflen) { \
		  string_part *last = __ctrl->parts_list_last; \
		  printf("fleece @7: adding escaped string of char length %zd, byte length %zd\n", __len, __buflen); \
	  	  memcpy_escaped(dynp(last), __add, __len, __buflen); \
	  	  last->len += __buflen; \
	  	  __ctrl->total_len += __buflen; \
		  printf("fleece @7: buffer %p buffer length now %zd, total length now %zd.\n", \
		  	last, last->len, __ctrl->total_len); \
	  	  DELIMIT(last) \
	    }
#else
#define bufcpy_escaped(__ctrl, __add, __len, __buflen) { \
		  string_part *last = __ctrl->parts_list_last; \
	  	  memcpy_escaped(__ctrl, dynp(last), __add, __len, __buflen); \
	  	  last->len += __buflen; \
	  	  __ctrl->total_len += __buflen; \
	  	  DELIMIT(last) \
	  	}
#endif

/* note: even if _add and _len are 0, it can be mandatory to check for the +2 ... ! */
#if SELFCHECK >= 1
#define buffer_add_string(_ctrl, _add, _len) {\
	  size_t _xlen = (_len); \
	  if(strlen(_add) != _xlen) { \
		printf(" ** fleece [1]: selfcheck remedies wrong string length, " \
			"%zd provided, actual %zd at string add macro call. " \
			"Must fix, will crash when run w/o selfcheck!\n", \
			_xlen, strlen(_add)); \
	    _xlen = strlen(_add); \
	  } \
	  if(_xlen + 3 >= _ctrl->parts_list_last->bufsize - _ctrl->parts_list_last->len) { \
			buffer_add(_ctrl, _xlen + 3); \
	  }\
	  if(_add && _xlen) { \
	  	bufcpy(_ctrl, _add, _xlen); \
	} } 
#else
#define buffer_add_string(_ctrl, _add, _xlen) { \
	  if(_xlen + 3 > _ctrl->parts_list_last->bufsize - _ctrl->parts_list_last->len) { \
			buffer_add(_ctrl, _xlen + 3); \
	  }\
	  if(_add && _xlen) { /* every value must assert at least 3 space, even if empty */\
	  	bufcpy(_ctrl, _add, _xlen); \
	} } 
#endif 

/* note: even if _add and _len are 0, it can be mandatory to check for the +2 ... ! */
#ifndef FLEECE_ASM
  #if SELFCHECK >= 1
    #define buffer_add_string_escaped(_ctrl, _add, _len) {\
	  size_t _xlen = (_len); \
	  size_t needs; \
	  count_escaped(_ctrl, _add, _xlen, needs); \
	  if(strlen(_add) != _xlen) { \
		printf(" ** fleece [1]: selfcheck remedies wrong string length, " \
			"%zd provided, actual %zd at string add macro call. " \
			"Must fix, will crash when run w/o selfcheck!\n", \
			_xlen, strlen(_add)); \
	    _xlen = strlen(_add); \
	  } \
	  if(_xlen + needs + 3 >= _ctrl->parts_list_last->bufsize - _ctrl->parts_list_last->len) { \
			buffer_add(_ctrl, _xlen + needs + 3); \
	  }\
	  if(needs)	{ /* implies _add & _xlen */\
	  	bufcpy_escaped(_ctrl, _add, _xlen, _xlen + needs); \
	  } \
	  else if(_add && _xlen) { \
	  	bufcpy(_ctrl, _add, _xlen); \
	} } 
	
  #else // SELFCHECK = 0, ASM undefined:

    #define buffer_add_string_escaped(_ctrl, _add, _len) {\
	  size_t _xlen = (_len); \
	  size_t needs; \
	  count_escaped(_ctrl, _add, _xlen, needs); \
	  if(_xlen + needs + 3 >= _ctrl->parts_list_last->bufsize - _ctrl->parts_list_last->len) { \
			buffer_add(_ctrl, _xlen + needs + 3); \
	  }\
	  if(needs)	{ /* implies _add & _xlen */\
	  	bufcpy_escaped(_ctrl, _add, _xlen, _xlen + needs); \
	  } \
	  else if(_add && _xlen) { \
	  	bufcpy(_ctrl, _add, _xlen); \
	} } 
// TODO: implement C memcntcpy

  #endif // < SELFCHECK

#else // ASM >

// TODO: the following, with self check!
#define buffer_add_string_escaped(_ctrl, _add, _xlen) { \
	  size_t needs; \
	  size_t sum; \
	  /* E2 */ \
	  if(_ctrl->escape_flag == 2) { \
	    if_buffer_remains(_ctrl, _xlen * 6 /* replaces *6 with *8 */) { \
	        /* if(*(_add + _xlen) != 0) { printf(" ** fleece: missing terminating 0 in Lua source stirng.\n"); exit(166); } */\
	  	    strcpy_escaped_2(dynp(_ctrl->parts_list_last), (_add), (_xlen),     sum); \
		    _ctrl->parts_list_last->len += sum; \
			_ctrl->total_len += _xlen + sum; \
	    } \
	    else {  \
     	  	buffer_assert(_ctrl, _xlen + RESERVE_BUFFER + 3); \
	      	memcntcpy_escaped_2(dynp(_ctrl->parts_list_last), _add, _xlen, needs); \
	        if(needs<=RESERVE_BUFFER)	/* then, memcpycnt did not cancel the copying and only counted */\
			    { _ctrl->parts_list_last->len += _xlen + needs; \
			      _ctrl->total_len += _xlen + needs; \
			    } \
    		else \
	    	  	bufcpy_escaped(_ctrl, _add, _xlen, _xlen + needs); \
    	  } \
	  } \
	  else \
	  /* E3 */ \
	  if(_ctrl->escape_flag == 3) { \
	    if_buffer_remains(_ctrl, _xlen * 6 /* replaces *6 with *8 */) { \
	        /* if(*(_add + _xlen) != 0) { printf(" ** fleece: missing terminating 0 in Lua source stirng.\n"); exit(166); } */\
	  	    strcpy_escaped_3(dynp(_ctrl->parts_list_last), (_add), (_xlen), sum); \
		    _ctrl->parts_list_last->len += sum; \
			_ctrl->total_len += _xlen + sum; \
	    } \
	    else {  \
     	  	buffer_assert(_ctrl, _xlen + RESERVE_BUFFER + 3); \
	      	memcntcpy_escaped_3(dynp(_ctrl->parts_list_last), _add, _xlen, needs); \
	        if(needs<=RESERVE_BUFFER)	/* then, memcpycnt did not cancel the copying and only counted */\
			    { _ctrl->parts_list_last->len += _xlen + needs; \
			      _ctrl->total_len += _xlen + needs; \
    			} \
	    	else \
		      	bufcpy_escaped(_ctrl, _add, _xlen, _xlen + needs); \
    	  } \
   	  } \
	  /* E4 */ \
	  else if(_ctrl->escape_flag == 4) { \
	    if_buffer_remains(_ctrl, _xlen << 3 /* replaces *6 with *8 */) { \
	        /* if(*(_add + _xlen) != 0) { printf(" ** fleece: missing terminating 0 in Lua source stirng.\n"); exit(166); } */\
	  	    strcpy_escaped_4(dynp(_ctrl->parts_list_last), (_add), (_xlen), sum); \
		    _ctrl->parts_list_last->len += sum; \
			_ctrl->total_len += _xlen + sum; \
	    } \
	    else {  \
     	  	buffer_assert(_ctrl, (_xlen) + RESERVE_BUFFER + 2); \
	      	memcntcpy_escaped_4(dynp(_ctrl->parts_list_last), (_add), (_xlen), needs); \
	        if(needs<=RESERVE_BUFFER) { \
                /* memcpycnt did not cancel the copying - so we are done */\
		        _ctrl->parts_list_last->len += (_xlen) + needs; \
			    _ctrl->total_len += (_xlen) + needs; \
		    } \
            /* memcpycnt canceled the copying and only counted */\
		    else { \
		  	    bufcpy_escaped(_ctrl, _add, _xlen, _xlen + needs); \
		    } \
		} \
	  } \
	  else \
	  /* ? */ \
	  { \
	  if(_add && _xlen) { /* every value must assert at least 3 space, even if empty */\
	  	bufcpy(_ctrl, _add, _xlen); \
	  } \
	  else { \
		  printf(" ** fleece: bad copy setting?"); exit(167); \
	  } \
	} \
  }
#endif 

/** blind character adding
 *  ----------------------
 *  Adds a character to the string buffer, blindly. In essence trusting that
 *  previous implicit logic will have supplied at least the one needed byte.
 *  This is rather laboriously tested for. It does make a difference in regard
 *  of performance.
 */
#if SELFCHECK >= 1
  #define buffer_add_char_blindly(_ctrl, _add) \
		{ if(_ctrl->parts_list_last->len + 1/* not +3 */ >= _ctrl->parts_list_last->bufsize) { \
			printf(" ** fleece [1]: selfcheck prevents buffer overrun error (%zd >= %zd) " \
				"with next(!) blind char add of '%c'. Did not happen yet. " \
				"Must fix, will crash when compiled w/o selftests!\n", \
				_ctrl->parts_list_last->len + 3, _ctrl->parts_list_last->bufsize, _add); \
			buffer_add(_ctrl, 0 + 3); \
			exit(195); \
		  }	\
		  buffer_add_char(_ctrl,_add); \
	    }
#else
  #define buffer_add_char_blindly(_ctrl, _add) buffer_add_char(_ctrl,_add)
#endif

#if VERBOSITY >= 9
  #define buffer_add_char(__ctrl,__add) { \
		  printf(" - add char '%c' - ", __add); \
	      *dynp(__ctrl->parts_list_last) = __add; \
	      __ctrl->parts_list_last->len++; \
	      __ctrl->total_len++; }
#else
  #define buffer_add_char(__ctrl,__add) { \
	      *dynp(__ctrl->parts_list_last) = __add; \
	      (__ctrl->parts_list_last->len)++; \
	      (__ctrl->total_len)++; } 
#endif

/** safe character appending
 *  ------------------------
 *  Adds a character to the string buffer, but checks for sufficient space in
 *  the buffer, first. I.e. comparing buffer size and max size. This does make
 *  dent into performance and is used only where it simply has to be, because
 *  no assumption could be made regarding remaining space in the buffer.
 *
 *  In the end, it uses the same primitive as the blind adding function, see above.
 */

#define buffer_add_char_safely(_ctrl, _add) {\
			if( 3 >= _ctrl->parts_list_last->bufsize - _ctrl->parts_list_last->len) { \
				buffer_add(_ctrl, 0 + 3); \
			} \
			buffer_add_char(_ctrl, _add); \
	    }

/** buffer remaining
 *  ----------------
 * Makes sure then at least _len free bytes in the buffer are available.
 * Allocates a new buffer if not, with at least _len space free.
 */
#if VERBOSITY >= 9
  #define if_buffer_remains(__ctrl, __len) {\
	printf("fleece @9: check for %zd bytes remaining free buffer: %zd free of %zd total\n", \
		__len, __ctrl->parts_list_last->bufsize - __ctrl->parts_list_last->len, \
		__ctrl->parts_list_last->bufsize); } \
	if((__len) + 3 <= __ctrl->parts_list_last->bufsize - __ctrl->parts_list_last->len) 
#else // < VERBOSITY 
  #define if_buffer_remains(__ctrl, __len) \
	if((__len) + 3 <= __ctrl->parts_list_last->bufsize - __ctrl->parts_list_last->len) 
#endif // < VERBOSITY 

/** buffer assertion
 *  ----------------
 * Makes sure then at least _len free bytes in the buffer are available.
 * Allocates a new buffer if not, with at least _len space free.
 */
#if VERBOSITY >= 9
  #define buffer_assert(_ctrl, _len) {\
	printf("fleece @9: assert %zd bytes remaining free buffer: %zd free of %zd total\n", \
		_len, _ctrl->parts_list_last->bufsize - _ctrl->parts_list_last->len, \
		_ctrl->parts_list_last->bufsize); \
	if(_len + 3 >= _ctrl->parts_list_last->bufsize - _ctrl->parts_list_last->len) { \
		printf("fleece @9: assert triggered new buffer creation\n"); \
		buffer_add(_ctrl, _len + 3); \
	} }
#else  
  #define buffer_assert(_ctrl, _len) { \
	if((_len) + 3 >= _ctrl->parts_list_last->bufsize - _ctrl->parts_list_last->len) { \
		buffer_add(_ctrl, (_len) + 3); \
	} }
#endif  

/*****************************************************************************\
***                                                                         ***
***                       ESCAPED MEMCPY: MAIN SWITCH                       ***
***                                                                         ***
\*****************************************************************************/
/* TODO: still in use?                                                       */

#define memcpy_escaped(_ctrl, _dst, _src, _len, _bufsize) { \
	switch(_ctrl->escape_flag) { \
		case 2: memcpy_escaped_2(_dst, _src, _len, _bufsize); break; \
		case 3: memcpy_escaped_3(_dst, _src, _len, _bufsize); break; \
		case 4: memcpy_escaped_4(_dst, _src, _len, _bufsize); break; \
		default: { printf(" ** fleece: bad count flag in escaped copy routine: %d\n", _ctrl->escape_flag); exit(188); } \
	} \
}

/*****************************************************************************\
***                                                                         ***
***                     ESCAPED COUNTING: MAIN SWITCH                       ***
***                                                                         ***
\*****************************************************************************/

#ifndef FLEECE_ASM
#define count_escaped(_ctrl, _s, _len, _needs) { \
	switch(_ctrl->escape_flag) { \
		case 2: _needs = count_escaped_2(_ctrl, _s, _len); break; \
		case 3: _needs = count_escaped_3(_ctrl, _s, _len); break; \
		case 4: _needs = count_escaped_4(_ctrl, _s, _len); break; \
		default: { printf(" ** fleece: bad count flag in escape count routine: %d\n", _ctrl->escape_flag); exit(187); } \
	} \
}
#endif 

#endif // < FLEECE_BUFFERS_H

