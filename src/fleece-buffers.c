/**-------------------------------------------------------------------------***
*** Packages    : Fleece - fast Lua to JSON conversion                      ***
*** File        : fleece-buffers.c                                          ***
*** Description : custom string buffer for fast JSON string compilation     ***
*** Version     : 0.2.2 / alpha                                             ***
*** Requirement : self sufficient ANSI C                                    ***
*** Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :    Feb 2011                                               ***
*** Changed     : 03 Mar 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Fleece is optimized for the fastest Lua to JSON conversion and beats   ***
***  other JSON implementations by around 10 times, native Lua up to 100x.  ***
***                                                                         ***
***  These string macros and functions were made specifically for the task  ***
***  of building a JSON string and speed things up considerably. There are  ***
***  trial and error solutions involved and I'll be happy to hear from you  ***
***  if you can suggest a better implementation or find that things go      ***
***  South on your platform.                                                ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  See header for essential macros. Header files not cleaned up.          ***
***                                                                         ***
***-------------------------------------------------------------------------**/

#include "fleece-buffers.h"

string_part *lucky = 0;
	
inline void fast_memcpy(char *dest, const char* src, size_t count) {

	char* dst8 = (char*)dest;
	char* src8 = (char*)src;
    --src8;
    --dst8;
        
    while (count--)
       *++dst8 = *++src8;
}

inline void first_part(insp_ctrl *ctrl)
{
	ctrl->parts_list_first = 0;
	ctrl->parts_list_last  = 0;
	ctrl->total_len        = 0;
	ctrl->total_parts      = 0;
	ctrl->total_waste      = 0;

	buffer_add(ctrl, 0,0,0,0,0,0);
	ctrl->parts_list_first = ctrl->parts_list_last;	
}

/* copy the string and hold it, in one dedicated buffer */
inline void add_dyn_part(string_part **last, const char *add, size_t len, size_t *total_len, 
	size_t *total_parts, size_t *waste, size_t buf_size)
{
	printf("add_dyn_part should not be used any more"); exit(1);

	// reuse & append last
	if(last && *last && (*last)->bufsize && (*last)->bufsize > (*last)->len && ((*last)->bufsize - (*last)->len) > len) 
	{
		#if (VERBOSITY >= 3)
    	printf(" reuse (%zd %zd %zd %p) ", (*last)->bufsize, (*last)->len, len, *last);
		#if (VERBOSITY >= 4)
		printf(" '%s' ", add);
		#endif
    	#endif

	    MEMCPY(dynp(*last), add, len);
	    (*last)->len += len;
		*(dynp(*last)) = 0;
    	*total_len = *total_len + len;
    	
		#if (SELFCHECK > 0)
    	if((*last)->bufsize < (*last)->len) {
    		printf(" ** buffer overflow: buffer %zd, length %zd\n", (*last)->bufsize, (*last)->len);
    		exit(1);
    	}
    	#endif
	}
	// create a new buffer
	else
	{
		#if (VERBOSITY >= 1)
		printf(" new ");
		#if (VERBOSITY >= 4)
		printf(" '%s' ", add);
		#endif
		#endif

		size_t use = (len > buf_size ? len : buf_size) + 1;
	    string_part *part = malloc(sizeof(string_part) + use);
		#if (VERBOSITY >= 6)
	    printf(" (ME: %p size %zd) ", part, use);
	    #endif
	    part->next = (string_part *)0;
	    part->len = len;
	    part->bufsize = use;
	    part->buf = (char *)0;
	    char *str = (char *)(part + 1);
	    MEMCPY(str, add, len);
    	str[len] = 0;

		// waste, if last was a dyn, not fully used.
		if(last && *last && (*last)->bufsize && (*last)->bufsize > (*last)->len) {
			*waste += (*last)->bufsize - (*last)->len;
			#if (VERBOSITY >= 3)
			printf(" waste: %zd ", *waste);
			#endif
		}
		
		if(*last) (*last)->next = part;
		#if (VERBOSITY >= 6)
		if(*last) printf("CHAIN: %p -> %p", *last, part); else  printf("NO LAST");
		#endif
		*last = part;
	    *total_len += len;
		(*total_parts)++;

		#if (VERBOSITY >= 3)
    	printf(" new %zd ", use);
		#endif
    }
    
	#if (VERBOSITY >= 5)
    printf(" (total len: %ld) ", *total_len);
    #endif
}


/* copy just the pointer and hold it, in one dedicated buffer */
inline void add_fix_part(string_part **last, const char const *add, size_t len, size_t *total_len,
	size_t *total_parts, size_t *waste)
{
	printf("add_fix_part should not be used any more"); exit(1);
	add_dyn_part(last, add, len, total_len, total_parts, waste, 20000); // TODO 20000! hack
}

/* copy just the pointer and hold it, in one dedicated buffer */
inline void add_fix_part_fix(string_part **last, const char const *add, size_t len, size_t *total_len,
	size_t *total_parts, size_t *waste)
{
	printf("add_fix_part_fix should not be used any more"); exit(1);

	#if (VERBOSITY >= 3)
   	printf(" new fix %zd ", len);
	#if (VERBOSITY >= 4)
	printf(" '%s' ", add);
	#endif
	#endif
		
	string_part *part = malloc(sizeof(string_part));
	#if (VERBOSITY >= 6)
    printf(" (MX: %p) ", part);
	#endif
    part->next = (string_part *)0;
    part->len = len;
    part->buf = (char *)add;
    part->bufsize = 0; // signal to add_dyn_part() re-use check

	// waste, if last was a dyn, not fully used.
	if(last && *last && (*last)->bufsize && (*last)->bufsize > (*last)->len) {
		*waste += (*last)->bufsize - (*last)->len;
		#if (VERBOSITY >= 3)
		printf(" waste: %zd ", *waste);
		#endif
	}
	
	if(*last) (*last)->next = part;
	#if (VERBOSITY >= 6)
	if(*last) printf("CHAIN: %p -> %p", *last, part); else  printf("NO LAST");
	#endif
    *total_len += len;
	*last = part;
	(*total_parts)++;
    
	#if (VERBOSITY >= 5)
    printf(" (total len: %ld) ", *total_len);
    #endif
}

inline char *collapse_parts(string_part **first, size_t total_len, size_t pparts_control)
{
	
	char *all, *p; 
	p = all = (char *)malloc(total_len+1);
	char *p_control = all + total_len + 1;
    string_part *next, *part; part = *first;
    size_t parts_control = pparts_control;
    
	#if (VERBOSITY >= 2)
	printf("fleece: collapse %zd parts, with expected length of %zd\n", parts_control, total_len);
	int count = 0;
	#endif
    
	while(part && parts_control--) {

		if(part->len >= total_len) {
			printf(" ** program error in fleece buffers, part len %zd > total len %zd (part #%zd of %zd)\n", 
				part->len, total_len, parts_control +1, pparts_control);
			printf(" total_len %zd, parts_control %zd, pparts_control %zd, first %p, part %p\n",
				total_len, parts_control, pparts_control, first, part);
			string_part *p = *first; int count = 0;
			while(p) { printf("#%d %p len: %zd\n", count++, p, p->len); p = p->next; }
			exit(998);
		}

		if(p + part->len >= p_control) {
			printf(" ** program error in fleece buffers (%p, %p, %zd, %p, %p)\n", 
				all, p, part->len, p + part->len, p_control);
			printf(" total_len %zd, parts_control %zd, pparts_control %zd, first %p, part %p\n",
				total_len, parts_control, pparts_control, first, part);
			exit(999);
		}

		#if (VERBOSITY >= 5)
		count++;
    	if(part->buf) printf("fleece: collapse part #%d:fix \"%s\", %zd len\n", count, part->buf, part->len);
        else printf("fleece: collapse part #%d: dyn %p \"%s\", %zd len. Next: %p\n", count, part, (char *)(part + 1), part->len, part->next);
		#endif

        if(part->buf) { MEMCPY(p, part->buf, part->len); }
        else { MEMCPY(p, (char *)(part + 1), part->len); }
        p += part->len;
		next = part->next;
		// free, except if the lucky buffer: keep that
        if(part != lucky) free(part); { *(char *)lucky = 0; *(char *)(lucky+1) = 0; }
        part = next;
	}
	*(p++) = 0;
	*first = (string_part *)0;

	#if (SELFCHECK >= 1)
		int error = 0;
		if(p != p_control) { printf(" ** fleece lost %d len (%40s)\n", p_control - p, all); error++; } 
		if(parts_control) { printf(" ** fleece lost %ld parts (%40s)\n", parts_control, all); error++; }
		if(error) { printf("Error. part %p, parts_control %d, p %p, p_control %p, part->len %x, => %p",
			part, parts_control, p, p_control, part->len,  p + part->len);
		}	
		#if (VERBOSITY >= 1)
			if(!((p != p_control) || parts_control)) printf("fleece: length & parts collapsed ok.\n");
		#endif
	#endif

	#if (VERBOSITY >= 5)
		printf("\ncollapse complete\n\n");
	#endif
	
	return all;		
}

/* collapse does that on its own */
inline size_t free_parts(string_part *first)
{
	size_t count = 0;
    string_part *next, *part = first;
	while(part) {
 		next = part->next;
		// free, except if the lucky buffer: keep that
        if(part != lucky) free(part); else { *(char *)lucky = 0; *(char *)(lucky+1) = 0; }
        part = next;
        count++;
	}
	return count;		
}

inline void free_collapsed(void *part)
{
	// free, except if the lucky buffer: keep that
       if(part != lucky) free(part); else { *(char *)lucky = 0; *(char *)(lucky+1) = 0; }
}


/* copy the string and hold it, in one dedicated buffer */
inline void buffer_add(insp_ctrl *ctrl, char add1, char add2, const char *add3, char add4, char add5, size_t len3)
{
	// printf("bufferadd: for %c%c%s%c%c\n", add1?add1:32, add2?add2:32, add3, add4?add4:32, add5?add5:32); \

	size_t len = (add1?1:0) + (add2?1:0) + len3 + (add4?1:0) + (add5?1:0);
	string_part *last = ctrl->parts_list_last;
	size_t last_bufsize = last ? last->bufsize : 0;

	// bottle last buffer
	#ifndef FLEECE_ALWAYS_DELIMIT /* if NOT def! */
	if(last) *(dynp(last)) = 0;
	#endif


		#if (VERBOSITY >= 1)
		printf(" new ");
		#if (VERBOSITY >= 4)
		printf(" '%s' '%s' '%s' '%s' '%s' ", add1, add2, add3, add4, add5); 
		#endif
		#endif

		string_part *part;
		size_t size;
		// first buffer: re-use if still around
		if(!last) {
			size = (len > ctrl->lucky_buf ? len : ctrl->lucky_buf) + 3;
			if(!lucky) lucky = malloc(sizeof(string_part) + size);
			part = lucky;
		}
		else
		{
			size = (len > ctrl->parts_buf ? len : ctrl->parts_buf) + 3; // TODO: check. MUST be len +3!! ("contract")
	    	part = malloc(sizeof(string_part) + size);
		}

		#if (VERBOSITY >= 6)
	    printf(" (ME: %p - size %zd) ", part, size);
	    #endif

	    part->next = (string_part *)0;
	    part->len = 0;
	    part->bufsize = size;
	    part->buf = (char *)0;
	    if(add1) { *dynp(part) = add1; part->len++; }
	    if(add2) { *dynp(part) = add2; part->len++; }
	    if(add3) { MEMCPY(dynp(part), add3, len3); part->len += len3; }
	    if(add4) { *dynp(part) = add4; part->len++; }
	    if(add5) { *dynp(part) = add5; part->len++; }
		#ifdef FLEECE_ALWAYS_DELIMIT
		*(dynp(part)) = 0;
		#endif
	    ctrl->total_len += len; /* (works here as len started out 0 some lines above) */

		// waste, if last was a dyn, not fully used.
		if(last && last_bufsize && last_bufsize > last->len) {
			ctrl->total_waste += last_bufsize - last->len;
			#if (VERBOSITY >= 3)
			printf(" waste: %zd ", ctrl->total_waste);
			#endif
		}
		
		// chain list
		if(last) last->next = part;

		#if (VERBOSITY >= 6)
		if(last) printf("CHAIN: %p -> %p", last, part); else  printf("NO LAST");
		#endif

		ctrl->parts_list_last = last = part;
		ctrl->total_parts++;
    
	#if (VERBOSITY >= 5)
    printf(" (total len: %ld) ", ctrl->total_len);
    #endif
}

/* this is pretty new and except for massive scatter shot not fully tested */
inline void buffer_rewind(insp_ctrl *ctrl, insp_ctrl *store, size_t start_len)
{
	string_part *start_part = store->parts_list_last;

	if(!start_part) { printf("Buffer rewind fails missing start buffer pointer"); exit(21); }
	if(start_part->buf) { printf("Buffer rewind fails with fixed as start buffer"); exit(22); }
	if(start_part->bufsize < start_len /* no - for unsigned! */) { printf("Buffer rewind fails with implausible buffer length sizes: bufsize %zd < len: %zd", start_part->bufsize, start_len); exit(23); }
	if((start_part->next == 0) && (start_part != ctrl->parts_list_last)) { printf("Buffer rewind fails with ctrl holding start part as current but start part has next"); exit(24); }
	if((start_part->next != 0) && (start_part == ctrl->parts_list_last)) { printf("Buffer rewind fails with ctrl not holding start part as current but start part has no next"); exit(25); }
	if(start_part->next == lucky) { printf("Buffer rewind fails with start part sporting lucky buffer as next"); exit(26); }
	
	// free potential new buffers
	int cf = 0;
	if(start_part->next) cf = free_parts(start_part->next);
	if(cf != ctrl->total_parts - store->total_parts) { printf("Buffer rewind fails with discrepancy in expected resulting total parts: %zd != %zd + %d", ctrl->total_parts, store->total_parts, cf); exit(27); }
	
	// RESET
	start_part->next = 0;
	start_part->len = start_len;
	*dynp(start_part) = 0; // just so
	memcpy(ctrl, store, sizeof(insp_ctrl));
	if(start_part != ctrl->parts_list_last) { printf("Buffer rewind fails with bad last pointer re-instation %p != %p", start_part, ctrl->parts_list_last); exit(28); }
}