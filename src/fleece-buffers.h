/**-------------------------------------------------------------------------***
*** Packages    : Fleece - fast Lua to JSON module                          ***
*** File        : fleece-buffers.h                                          ***
*** Description : header, incl. main macros for custom string buffers       ***
*** Version     : 0.2.2 / alpha                                             ***
*** Requirement : self sufficient ANSI C                                    ***
*** Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :    Feb 2011                                               ***
*** Changed     :    Feb 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Header files not cleaned up.                                           ***
***                                                                         ***
***-------------------------------------------------------------------------**/

#ifndef FLEECE_BUFFERS_H
#define FLEECE_BUFFERS_H 1

#define VERBOSITY 0 // 0-6
#define SELFCHECK 0 // 0-2

// original memcpy is faster than C on faster computers, except Duffy style
#define MEMCPY fast_memcpy_macro

/* collect string parts, dynamic, or fix.  Dynamic is stored post-adjacent. */
struct string_part_struct { 
	struct string_part_struct *next;
	char *buf; /* for fixed */
	size_t len;
	size_t bufsize;
};

#define is_fixed_buffer(part) ((part)->buf != 0)

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

#define fast_memcpy_macro(_dest, _src, _count) { \
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


/* collect string parts, dynamic, or fix.  Dynamic is stored post-adjacent. */
typedef struct string_part_struct string_part;

#include "fleece-insp_ctrl.h"

inline void first_part(insp_ctrl *ctrl);
inline void buffer_add(insp_ctrl *ctrl, char pre1, char pre2, const char *add, 
	char post1, char post2, size_t len);
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

/* a note: even if _add and _len are 0, it can be mandatory to check for the +2 ... ! */
#define buffer_add_string(_ctrl, _add, _len) \
	{ if(_len + 2 >= _ctrl->parts_list_last->bufsize - _ctrl->parts_list_last->len) { \
			buffer_add(_ctrl, 0, 0, 0, 0, 0, 0); \
	  }\
	  if(_add && _len) { \
		  string_part *last = _ctrl->parts_list_last; \
	  	  MEMCPY(dynp(last), _add, _len); \
	  	  last->len += _len; \
	  	  _ctrl->total_len += _len; \
	  	  DELIMIT(last) \
	} } 

#if SELFCHECK >= 1
#define buffer_add_char(_ctrl, _add) \
		{ if(_ctrl->parts_list_last->len + 2 >= _ctrl->parts_list_last->bufsize) { \
			printf(" ** selfcheck remedies buffer overrun (%zd >= %zd) at char add. Must fix!\n", \
				_ctrl->parts_list_last->len +2, _ctrl->parts_list_last->bufsize); \
			buffer_add(_ctrl, 0, 0, 0, 0, 0, 0); \
		  }	\
	      *dynp(_ctrl->parts_list_last) = _add; \
	      _ctrl->parts_list_last->len++; \
	      _ctrl->total_len++; \
	    }
#else
#define buffer_add_char(_ctrl, _add) \
	    { *dynp(_ctrl->parts_list_last) = _add; \
	      _ctrl->parts_list_last->len++; \
	      _ctrl->total_len++; }
#endif

#define buffer_assert(_ctrl, _len) \
	if(_len >= _ctrl->parts_list_last->bufsize - _ctrl->parts_list_last->len) { \
		buffer_add(_ctrl, 0, 0, 0, 0, 0, 0); \
	}

#endif