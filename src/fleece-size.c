/**-------------------------------------------------------------------------***
*** File        : Fleece - Lua to JSON module                               ***
*** File        : fleece-size.c                                             ***
*** Description : traverse Lua data and count the JSON string size needed   ***
*** Version     : 0.2.2 / alpha                                             ***
*** Requirement : Lua 5.1.4 - 5.1.4-2                                       ***
*** Copyright   : (c) 2011 Henning Diedrich, Eonblast Corporation           ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :    Feb 2011                                               ***
*** Changed     : 02 Mar 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Fleece is optimized for the fastest Lua to JSON conversion and beats   ***
***  other JSON implementations by around 10 times, native Lua up to 100x.  ***
***  This file ist not central but offers speed gains for its purpose.      ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Header files not cleaned up.                                           ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  This file is INCLUDED into fleece.c currently.                         ***
***                                                                         ***
***-------------------------------------------------------------------------**/

/* convenience macro */
#define add(__ctrl, __count) ((__ctrl)->total_len += (__count))

/* 'inline' macro of main function */
#define FLEECE_SIZE(__ctrl, __o) fleece_size_macro(__ctrl, __o)

/* declaration */
void fleece_size_hash_part (insp_ctrl *ctrl, const Table *t, size_t *count, int *pure);
void fleece_size_array_part (insp_ctrl *ctrl, const Table *t, size_t *count, int *pure);

/*
 * 'inline' implementation
 *
 */
#define fleece_size_macro(_ctrl, _o) \
{ \
	/* TODO: make switch */\
    if (ttisnil(_o)) \
    { \
		add(_ctrl, 4); \
    } \
    else if (ttisboolean(_o)) \
    { \
		if((_o)->value.b) { \
			add(_ctrl, 4); /* TODO fix? */\
		} \
		else { \
			add(_ctrl, 5); /* TODO fix? */\
		} \
    } \
    else if (ttisnumber(_o)) \
    { \
		int i = (int)nvalue(_o); \
		if(((double)i) == (nvalue(_o))) { \
			/* as integer */\
		    add(_ctrl, ilen(i)); \
		} \
		else { \
			/* as float */\
		    add(_ctrl, dlen(nvalue(_o), 7,'g')); /* TODO: flags */\
		} \
	} \
    else if (ttisstring(_o)) \
    { \
		add(_ctrl, tsvalue(_o)->len +2); \
    } \
    else if (ttistable(_o)) \
    { \
		add(_ctrl, 1); /* opening bracket */\
		size_t count = 1; \
		int pure = 1; \
		fleece_size_array_part(_ctrl, hvalue(_o), &count, &pure); \
		fleece_size_hash_part(_ctrl, hvalue(_o), &count, &pure); \
    } \
    else if (ttisfunction(_o)) \
    { \
	    add(_ctrl, sizeof(LIT_FUNCTION)); \
    } \
    else if (ttisuserdata(_o)) \
    { \
	    add(_ctrl, sizeof(LIT_USER_DATA)); \
    } \
    else if (ttisthread(_o)) \
    { \
	    add(_ctrl, sizeof(LIT_THREAD)); \
    } \
    else if (ttislightuserdata(_o)) \
    { \
	    add(_ctrl, sizeof(LIT_LIGHT_USER_DATA)); \
    } \
    else \
    { \
	    add(_ctrl, sizeof(LIT_TYPELESS)); \
    } \
} \

/*
 * main function for size calculation 
 *
 */
void fleece_size_(insp_ctrl *ctrl, const TValue *o) {

	fleece_size_macro(ctrl, o);
	
}

void fleece_size_array_part (insp_ctrl *ctrl, const Table *t, size_t *count, int *pure) {
  int lg;
  int ttlg;  /* 2^lg */
  int i = 1;  /* count to traverse all array keys */
  for (lg=0, ttlg=1; lg<=MAXBITS; lg++, ttlg*=2) {  /* for each slice */
    int lim = ttlg;
    if (lim > t->sizearray) {
      lim = t->sizearray;  /* adjust upper limit */
      if (i > lim)
        break;  /* no more elements to count */
    }
    /* count elements in range (2^(lg-1), 2^lg] */
    for (; i <= lim; i++) {   
		TValue * v = &t->array[i-1];
		if(!ttisnil(v)) {
			if((*count)++ != i) add(ctrl, ilen(i)); 
			ctrl->total_len += 1; // :
			FLEECE_SIZE(ctrl, v);
		}
    }
  }
}

void fleece_size_hash_part (insp_ctrl *ctrl, const Table *t, size_t *count, int *pure) {

  int i = sizenode(t);
  while (i--) {
	Node *node = &t->node[i];
	if(!ttisnil(key2tval(node)) && !ttisnil(gval(node))) {
		FLEECE_SIZE(ctrl, (const TValue *)gkey(node));
		FLEECE_SIZE(ctrl, (const TValue *)gval(node));
		ctrl->total_len += 2; // : , or (rsp closing bracket!)
	}
  } 
}
