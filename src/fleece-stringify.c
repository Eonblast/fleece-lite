/**-------------------------------------------------------------------------***
*** File        : Fleece - Lua to JSON module                               ***
*** File        : fleece-stringify.c                                        ***
*** Description : Traverse Lua data and create the JSON string              ***
*** Version     : 0.3.0 / alpha                                             ***
*** Requirement : Lua 5.1.4 - 5.1.4-2                                       ***
*** Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :    Feb 2011                                               ***
*** Changed     : 19 Mar 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Fleece is optimized for the fastest Lua to JSON conversion and beats   ***
***  other JSON implementations by around 10 times, native Lua up to 100x.  ***
***                                                                         ***
***  This file holds some of the basis for the great speed though strings,  ***
***  especially when escaped, are 'only' around 7 times faster.             ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Header files not cleaned up. There are multiple strategies visible     ***
***  in this source, some will be abandoned and cleaned up. Basically:      ***
***                                                                         ***
***  --- buffer ---                                                         ***
***  A) count first, check remaining buffer, enlarge if nec., then copy     ***
***  B) check if remaining buffer < 'max escape case' (6x), enlarge, copy   ***
***  C) without any checks rely on assured reserve buffer for single chars  ***
***                                                                         ***
***  --- counting ---                                                       ***
***  I) plain count, for finding the destination size, then copy            ***
***  II) count and copy, cancel copying when exceeding assured reserve      ***
***  III) use built in C lib strcspn to count escape characters (--)        ***                                                                         ***                                                                         ***
***  --- copy control ---                                                   ***
***  i) memory-copy using given source length                               ***
***  ii) string-copy relying on final 0 (faster for ASM, not tru to Lua)    ***
***                                                                         ***
***  --- copying ---                                                        ***
***  1) use built in C lib memcpy                                           ***
***  2) use native C programmed memcpy (faster for old PPC) (--)            ***
***  3) use native C programmed, block pointer casting-memcpy               ***
***  4) use assembler (x86 only, 10 - 20% faster, more on older CPUs)       ***
***                                                                         ***
***  I'll be happy to learn about what you find to work best in your case.  ***  
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  This file is INCLUDED into fleece.c currently.                         ***
***                                                                         ***
***  Many function calls here are actually macros, mostly from buffers.h.   ***
***                                                                         ***
***-------------------------------------------------------------------------**/

/*****************************************************************************
***                                                                         ***
***                                DECLARATIONS                             ***
***                                                                         ***
 *****************************************************************************/ 

/* TODO: not cleaned up. Many functions have been made into macros */
void itoa(int value, char* str, size_t *rlen); // TODO move
char *fleece_stringify(insp_ctrl *ctrl, const TValue *o, void**freer);
void stringify_value(insp_ctrl *ctrl, const TValue *o);
void stringify_null(insp_ctrl *ctrl);
void stringify_boolean(insp_ctrl *ctrl, int value);
void stringify_integer(insp_ctrl *ctrl, int value);
void stringify_number(insp_ctrl *ctrl, double value);
void stringify_string(insp_ctrl *ctrl, const char* S, const size_t len);
void stringify_placeholder(insp_ctrl *ctrl, const char* S, const size_t len);
void stringify_table (insp_ctrl *ctrl, Table *t);
void stringify_array_part (insp_ctrl *ctrl, const Table *t, size_t *count, int *pure, int*tried, int force);
void stringify_array_part_element (insp_ctrl *ctrl, int i, TValue *v, size_t count);
void stringify_hash_part (insp_ctrl *ctrl, const Table *t, size_t *count, int *pure);
void stringify_node(insp_ctrl *ctrl, Node * n, size_t count);

/*****************************************************************************
***                                                                         ***
***                                 HELPER                                  ***
***                                                                         ***
 *****************************************************************************/ 

#if FLEECE_WITH_ESCAPE > 0

  #ifndef FLEECE_ASM

    #define buffer_add_string_possibly_escaped(__ctrl, __add, __len) \
		if(__ctrl->escape_flag < FLEECE_PURGE_FLAG) {\
			buffer_add_string(__ctrl, __add, __len); \
		} else if(__ctrl->escape_flag == FLEECE_PURGE_FLAG) {\
			buffer_add_string(__ctrl, __add, __len); \
			purge_escaped(dynp(__ctrl->parts_list_last) - (__len), __len); \
		} else { \
			buffer_add_string_escaped(__ctrl, __add, __len); \
		} 

  /* Assembler: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
  #else

    #define buffer_add_string_possibly_escaped(__ctrl, __add, __len) \
		if(__ctrl->escape_flag < FLEECE_PURGE_FLAG) {\
			buffer_add_string(__ctrl, __add, __len); \
		} else if(__ctrl->escape_flag == FLEECE_PURGE_FLAG) {\
			buffer_assert(__ctrl, __len +5); \
            asm_purgecpy(dynp(__ctrl->parts_list_last), __add, __len); \
            __ctrl->parts_list_last->len += __len; \
            __ctrl->total_len += __len; \
		} else { \
			buffer_add_string_escaped(__ctrl, __add, __len); \
		} 

  #endif // < ASM

/* NON-FLEECE_WITH_ESCAPE -> fastest, blind non-escaping: ------------------ */

#else

  #define buffer_add_string_possibly_escaped(__ctrl, __add, __len) \
			buffer_add_string(__ctrl, __add, __len); 

#endif // < FLEECE_WITH_ESCAPE

/*---------------------------------------------------------------------------*\
 ** Placeholder for special types: simply a string                          **
\*---------------------------------------------------------------------------*/
/* The definitions coming in through S are defined invariably in intern.h    */

void stringify_placeholder(insp_ctrl *ctrl, const char *S, const size_t len)
{
	buffer_add_string(ctrl, S, len);
}

/*****************************************************************************\
***                                                                         ***
***                            MAIN VALUE ENCODER                           ***
***                                                                         ***
 ***************************************************************************** 
 * This macro switches over types and calls the respective encoder functions *
 * for the values identified. It is itself inlined into fleece_stringify()   *
 * and inlines multiple buffer handling macros, with the exception of        *
 * function calls to the table handling functions.                           * 
\*****************************************************************************/

#define stringify_value_macro(_ctrl, _o) \
{ \
	/* TODO: make switch */\
    if (ttisnil(_o)) \
    { \
		buffer_add_string(_ctrl, LIT_NULL, 4); \
    } \
    else if (ttisboolean(_o)) \
    { \
		if((_o)->value.b) { \
			buffer_add_string(_ctrl, LIT_TRUE, 4); /* TODO fix? */\
		} \
		else { \
			buffer_add_string(_ctrl, LIT_FALSE, 5); /* TODO fix? */\
		} \
    } \
    else if (ttisnumber(_o)) \
    { \
		int i = (int)nvalue(_o); \
		if(((double)i) == (nvalue(_o))) { \
			/* as integer */\
			buffer_assert(_ctrl, 14); \
		    size_t len; \
		    itoa(i, dynp(_ctrl->parts_list_last), &len); \
		    _ctrl->parts_list_last->len += len; \
		    _ctrl->total_len += len; \
		} \
		else { \
			/* as float */\
		    buffer_assert(_ctrl, 34); \
		    size_t len;  \
   		    dtoa(nvalue(_o),dynp(_ctrl->parts_list_last),7,'g', &len); /* TODO: flags */\
		    _ctrl->parts_list_last->len += len; \
		    _ctrl->total_len += len; \
		} \
	} \
    else if (ttisstring(_o)) \
    { \
\
		buffer_add_char_blindly(_ctrl, '"'); \
		buffer_add_string_possibly_escaped(_ctrl, svalue(_o), tsvalue(_o)->len); \
		buffer_add_char_blindly(_ctrl, '"'); \
    } \
    else if (ttistable(_o)) \
    { \
		buffer_add_char_safely(_ctrl, '{'); \
		char *bracket = dynp(_ctrl->parts_list_last) -1; \
\
		size_t count = 1; \
		int pure = 1; \
		int tried = 0; \
		int force = 0; \
		insp_ctrl store; \
		/* TODO: assert? to avoid running into a fix here? does that matter? */\
		memcpy(&store, _ctrl, sizeof(insp_ctrl)); \
		size_t store_parts_buf_len = _ctrl->parts_list_last->len; \
		stringify_array_part(_ctrl, hvalue(_o), &count, &pure, &tried, force); \
		if(!tried || pure) stringify_hash_part(_ctrl, hvalue(_o), &count, &pure); \
		/* redo when started out w/o index but then encountered one one the way */\
		if(tried && !pure) { \
			/* rewind writes*/\
			buffer_rewind(_ctrl, &store, store_parts_buf_len); \
			count = 1; force = 1; \
			stringify_array_part(_ctrl, hvalue(_o), &count, &pure, &tried, force); \
			stringify_hash_part(_ctrl, hvalue(_o), &count, &pure); \
			pure = 0; /* switch for below */\
		} \
		if(count == 1) /* empty array = always array, not object */ { \
			buffer_add_char_safely(_ctrl, ']'); \
			*bracket='['; \
		} \
		else if(pure) /* array, not empty */ { \
			/* writes over last ',' */\
			*(dynp(_ctrl->parts_list_last) -1)=']'; \
			/* (*) must therefore assert buffer, as could be part of ]]]... */\
			buffer_assert(_ctrl, 0); /* sic */\
			*bracket='['; \
		} \
		else /* object */ { \
			/* writes over last ',' */\
			*(dynp(_ctrl->parts_list_last) -1)='}'; \
			/* (*) must therefore assert buffer, as could be part of }}}... */\
			buffer_assert(_ctrl, 0); /* sic */\
		} \
    } \
    else if (ttisfunction(_o)) \
    { \
	    stringify_placeholder(_ctrl, LIT_FUNCTION, sizeof(LIT_FUNCTION)); \
    } \
    else if (ttisuserdata(_o)) \
    { \
	    stringify_placeholder(_ctrl, LIT_USER_DATA, sizeof(LIT_USER_DATA)); \
    } \
    else if (ttisthread(_o)) \
    { \
	    stringify_placeholder(_ctrl, LIT_THREAD, sizeof(LIT_THREAD)); \
    } \
    else if (ttislightuserdata(_o)) \
    { \
	    stringify_placeholder(_ctrl, LIT_LIGHT_USER_DATA, sizeof(LIT_LIGHT_USER_DATA)); \
    } \
    else \
    { \
	    stringify_placeholder(_ctrl, LIT_TYPELESS, sizeof(LIT_TYPELESS)); \
    } \
} \

/*****************************************************************************\
***                                                                         ***
***                            HANDLING TABLES                              ***
***                                                                         ***
\*****************************************************************************/

/*---------------------------------------------------------------------------*\
 ** Stringify the array part of a table                                     **
 *---------------------------------------------------------------------------*
 * This is Lua 5.1.4 specific.                                               *
 * A table, internally, has a hash & an array part                           *
\*---------------------------------------------------------------------------*/

void stringify_array_part (insp_ctrl *ctrl, const Table *t, size_t *count, int *pure, int *tried, int force) 
{
	int lg;
	int ttlg;  /* 2^lg */
	int i = 1;  /* count to traverse all array keys */
	int try = 0; /* 1 := started out well to stay w/o a key = pure */
	int pu = 1;
	for (lg=0, ttlg=1; lg<=MAXBITS; lg++, ttlg*=2) {  /* for each slice */
		int lim = ttlg;
    if (lim > t->sizearray) {
		lim = t->sizearray;  /* adjust upper limit */
		if (i > lim)
        	break;  /* no more elements */
    }
    /* elements in range (2^(lg-1), 2^lg] */
    for (; i <= lim; i++) {
		TValue * v = &t->array[i-1];
		if(!ttisnil(v)) {
			if(force || *count != i) {
				//stringify_integer(ctrl, i); // TODO ... flags, pure arrays ...
			    buffer_assert(ctrl, 14); // TODO right buffer size here? (+2 o 3!)
			    size_t len; 
			    itoa(i, dynp(ctrl->parts_list_last), &len);
			    ctrl->parts_list_last->len += len;
			    ctrl->total_len += len;
				buffer_add_char_blindly(ctrl, ':'); // TODO ... flags, pure arrays ...
				if(try) { (*count)++; *pure = 0; *tried = 1; return; } /* cancel and redo */
				pu = 0;
			} else
				try = 1; /* we try to stay w/o key = pure */
			(*count)++;
			stringify_value_macro(ctrl, v);
			/* this must not go entirely blind but is taken care of here (*) */
			buffer_add_char_blindly(ctrl, ','); // TODO ... flags, pure arrays ...
			// TODO: should be ok, covered, with blindly
		}
    }
    *pure = pu;
    *tried = try;
    
  }
}

/*---------------------------------------------------------------------------*\
 * Stringify the hash part of a table                                        *
 *---------------------------------------------------------------------------*
 * This is Lua 5.1.4 specific.                                               *
 * A table, internally, has a hash & an array part                           *
\*---------------------------------------------------------------------------*/

void stringify_hash_part (insp_ctrl *ctrl, const Table *t, size_t *count, int *ppure) 
{
	int i = sizenode(t);
	int pure = *ppure;

	/* note: only by looping will we learn if there are elements here (?) */
	
	while (i--) {
		Node *node = &t->node[i];
		if(!ttisnil(key2tval(node)) && !ttisnil(gval(node))) {
			(*count)++;
			stringify_value_macro(ctrl, key2tval(node));
 			buffer_add_char_blindly(ctrl, ':');
			stringify_value_macro(ctrl, gval(node));
			buffer_add_char_safely(ctrl, ',');
			pure = 0;
		}
	}
	*ppure = pure;
}

/*****************************************************************************\
***                                                                         ***
***                           STRINGIFY ENTRY CALL                          ***
***                                                                         ***
 *****************************************************************************
 * main stringify function, called from the Lua-facing fleece_json(L)        *
\*---------------------------------------------------------------------------*/
char *fleece_stringify(insp_ctrl *ctrl, const TValue *o, void **freer)
{
	
	#if VERBOSITY >= 8
	printf("fleece_stringify()\n");
	#endif

	stringify_value_macro(ctrl, o);

	/* IMPORTANT: last 0, in case of not FLEECE_ALWAYS_DELIMIT. As the buffer is reused. */
	*(dynp(ctrl->parts_list_last)) = 0;

	char *str;

	if(ctrl->total_parts == 1 && !ctrl->parts_list_first->buf) // not fix
	{
		
		str = (char *)((ctrl->parts_list_first) + 1);
		*freer = ctrl->parts_list_first;
		ctrl->parts_list_first = 0; // to avoid freeing in free_insp_ctrl
		ctrl->had_parts = ctrl->total_parts;
		ctrl->total_parts = 0;
		ctrl->final_copy = 0;
	}
	else
	{
		
		str = collapse_parts(&ctrl->parts_list_first, ctrl->total_len, ctrl->total_parts);
		*freer = str;
		ctrl->had_parts = ctrl->total_parts;
		ctrl->total_parts = 0;
		ctrl->final_copy = 1;
	}
	
	return str;
}

