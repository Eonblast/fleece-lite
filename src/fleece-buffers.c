/**-------------------------------------------------------------------------***
*** Packages    : Fleece - fast Lua to JSON conversion                      ***
*** File        : fleece-buffers.c                                          ***
*** Description : custom string buffer for fast JSON string compilation     ***
*** Version     : 0.3.0 / alpha                                             ***
*** Requirement : self sufficient ANSI C                                    ***
*** Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :    Feb 2011                                               ***
*** Changed     : 05 Mar 2011                                               ***
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

#include "fleece-config.h"
#include "fleece-buffers.h"

/*---------------------------------------------------------------------------*\
**  Global Pointer to the first ("lucky") string holder buffer               **
\*---------------------------------------------------------------------------*/
string_part *lucky = 0;
	
/*---------------------------------------------------------------------------*\
**  Initializing the first of buffers                                        **
\*---------------------------------------------------------------------------*/
inline void first_part(insp_ctrl *ctrl)
{
	ctrl->parts_list_first = 0;
	ctrl->parts_list_last  = 0;
	ctrl->total_len        = 0;
	ctrl->total_parts      = 0;
	ctrl->total_waste      = 0;

	buffer_add(ctrl, 0);
	ctrl->parts_list_first = ctrl->parts_list_last;	
}

/*---------------------------------------------------------------------------*\
**  Add a new string holder buffer to the list of buffers                    **
\*---------------------------------------------------------------------------*/
inline void buffer_add(insp_ctrl *ctrl, size_t minlen)
{

	#if VERBOSITY >= 8
	printf("fleece *8: buffer_add(minlen: %zd)\n", minlen); 
	#endif

	string_part *last = ctrl->parts_list_last;
	size_t last_bufsize = last ? last->bufsize : 0;

	// bottle last buffer
	#ifndef FLEECE_ALWAYS_DELIMIT /* if NOT def! */
	if(last) *(dynp(last)) = 0;
	#endif

	string_part *part;
	size_t size;
	// first buffer: re-use if still around
	if(!last /* means, has no predecessor == is first of parts list */) {
		size = (minlen > ctrl->lucky_buf ? minlen : ctrl->lucky_buf);
		if(!lucky) lucky = malloc(sizeof(string_part) + size +1);
		part = lucky; /* think */
	}
	else
	{
		size = (minlen > ctrl->parts_buf ? minlen : ctrl->parts_buf);
	    	part = malloc(sizeof(string_part) + size +1);
	}
	*(((char *)part) + sizeof(string_part) + size + 1) = '$';
	
	if(size < 5) { printf("fleece: ** buffer < 5 exit.\n"); exit(196); }

	#if (VERBOSITY >= 6)
	printf(" fleece @6: new buffer %p, size %zd ", part, size);
	#endif

	/* buffer init */
	part->next = (string_part *)0;
	part->len = 0;
	part->bufsize = size;
	part->buf = (char *)0;
	#ifdef FLEECE_ALWAYS_DELIMIT
	*(dynp(part)) = 0;
	#endif

	// waste, if last was a dyn, not fully used.
	if(last && last_bufsize && last_bufsize > last->len) {
		ctrl->total_waste += last_bufsize - last->len;
		#if (VERBOSITY >= 3)
		printf(" fleece @3: waste %zd ", ctrl->total_waste);
		#endif
	}
		
	// chain list
	if(last) last->next = part;

	#if (VERBOSITY >= 6)
	if(last) printf(" fleece @6: buffer chain %p -> %p ", last, part); 
	else  printf(" fleece @6: no previous buffer ");
	#endif

	ctrl->parts_list_last = last = part;
	ctrl->total_parts++;
    
	#if (VERBOSITY >= 5)
	printf(" total len now: %ld ", ctrl->total_len);
	#endif
}

/*---------------------------------------------------------------------------*\
** Rewind and discard all string buffers back to a given point               **
\*---------------------------------------------------------------------------*/
/* This is used for that case that on the way it is discovered that a table  *
 * cannot be converted into a JSON array but must be a JSON object. In that  *
 * case, the entire encoding of the table is discarded, possibly across      *
 * buffers. None, one or many buffers may get discarded in the process.      *
 * TODO: except for massive scatter shot not tested by dedicated tests       * 
\*---------------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------------*\
** Final wrap up, copying all buffers into one.                              **
\*---------------------------------------------------------------------------*/
/* In case that there is only one buffer in use, this function is not even   *
 * called, but instead that buffer is used as result. See fleece_stringify().*
\*---------------------------------------------------------------------------*/
inline char *collapse_parts(string_part **first, size_t total_len, size_t pparts_control)
{
	
	char *all, *p; 
	p = all = (char *)malloc(total_len+1);
	char *p_control = all + total_len + 1;
	string_part *next, *part; part = *first;
	size_t parts_control = pparts_control;
    
	#if (VERBOSITY >= 2)
	printf("\nfleece @2: collapse into new buffer %p, %zd parts, " \
		"with expected length of %zd\n", all, parts_control, total_len);
	int count = 1;
	string_part *dumpp = *first; 
	while(dumpp) { printf("part buffer #%d %p size: %zd, len: %zd\n", 
		count++, dumpp, dumpp->bufsize, dumpp->len); dumpp = dumpp->next; }
	count = 0; /* used below */
	#endif
    
	while(part && parts_control--) {

		if(!part->buf /* means, dyn */ && part->len > part->bufsize) {
			printf("fleece [0]: ** program error in dynamic fleece buffers, " \
				   "part %p len %zd > buf size %zd (part #%zd of %zd, use VERBOSITY 2 for buffer dump)\n", 
				part, part->len, part->bufsize, parts_control +2, pparts_control);
			printf(" total_len %zd, parts_control %zd, pparts_control %zd, " \
				"first %p, part %p\n", total_len, parts_control, pparts_control,
				*first, part);
			exit(201);
		}

		if(part->len >= total_len) {
			printf("fleece [0]: ** program error in fleece buffers, part len %zd > total len %zd (part #%zd of %zd)\n", 
				part->len, total_len, parts_control +2, pparts_control);
			printf(" total_len %zd, parts_control %zd, pparts_control %zd, first %p, part %p\n",
				total_len, parts_control, pparts_control, first, part);
			exit(202);
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
    	if(part->buf) printf("fleece @5: collapse FIX part #%d: \"%s\", %zd len\n", count, part->buf, part->len);
        else printf("fleece @5: collapse dyn part #%d: %p \"%s\", len: %zd, buf size: %zd. Next: %p\n", count, part, (char *)(part + 1), part->len, part->bufsize, part->next);
		#endif

        // TODO: put 0 and use strcpy
        // note: changed to memcpy from MEMCPY 03/18
        // this is a special place that will deal with larger swathes than
        // most other *cpy places.
        if(part->buf) { memcpy(p, part->buf, part->len); } 
        else { memcpy(p, (char *)(part + 1), part->len); }
        p += part->len;
		next = part->next;
		
		// free, except if the lucky buffer: keep that
        if(part != lucky) { 
       		#if (VERBOSITY >= 9)
       		printf("free buffer %p\n", part);
       		#endif
			/* check post-end tag alive */
			if(*(((char *)part) + sizeof(string_part) + part->bufsize + 1) != '$') {
				printf("fleece [0]: ** buffer end tag  overrun (%zd/%zd): '%.*s'", part->len, part->bufsize, (int)part->bufsize +1, ((char *)part) + (sizeof(string_part)));
				exit(193);
			 }		
         	free(part); 
        } else { *(char *)lucky = 0; *(char *)(lucky+1) = 0; }
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
		printf("\ncollapse complete. Delivering pointer %p\n\n", all);
	#endif
	
	return all;		
}

/*---------------------------------------------------------------------------*\
 ** Free the obsolete string buffers                                        **
\*---------------------------------------------------------------------------*/
inline void free_collapsed(void *part)
{
	#if (VERBOSITY >= 8)
	printf("free_collapsed(%p): ", part); 
	#endif

	// free, except if the lucky buffer: keep that
    if(part != lucky) {
		#if (VERBOSITY >= 8)
		printf("free (lucky is %p)\n", lucky); 
		#endif
    		free(part); 
   }
   else { 
		#if (VERBOSITY >= 8)
		printf("not, is lucky.\n"); 
		#endif
   
		*(char *)lucky = 0; *(char *)(lucky+1) = 0; 
   }
}

/*---------------------------------------------------------------------------*\
 ** Free one obsolete string buffers                                        **
\*---------------------------------------------------------------------------*/
/* collapse() does that on its own                                           */
inline size_t free_parts(string_part *first)
{
	#if (VERBOSITY >= 8)
	printf("free_parts(first: %p)\n", first); 
	#endif
	size_t count = 0;
    string_part *next, *part = first;
	while(part) {
 		next = part->next;
		#if (VERBOSITY >= 8)
		printf("free_parts:  %p (next %p): ", part, next); 
		#endif
		// free, except if the lucky buffer: keep that
        if(part != lucky) {
			#if (VERBOSITY >= 8)
			printf("free\n"); 
			#endif
			/* check post-end tag alive */
           	if(*(((char *)(part)) + sizeof(string_part) + part->bufsize + 1) != '$') {
               	printf("fleece [0]: ** buffer end tag overrun (%zd/%zd): '%.*s'", part->len,
               		part->bufsize, (int) part->bufsize +1, ((char *)part) + (sizeof(string_part)));
       	        exit(192);  
   	         }
            free(part);
			printf("done");
        }
        else { 
			#if (VERBOSITY >= 8)
			printf("not, is lucky\n"); 
			#endif
        	*(char *)lucky = 0; *(char *)(lucky+1) = 0; 
        }
        part = next;
        count++;
	}
	return count;		
}

