/**-------------------------------------------------------------------------***
*** File        : Fleece - Lua to JSON module                               ***
*** File        : fleece-copy.c                                             ***
*** Description : Optimized counting, copying and escaping for JSON strings ***
*** Version     : 0.3.1 / alpha                                             ***
*** Requirement : self sufficient ANSI C + optional x86 ASM                 ***
*** Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     : 11 Mar 2011                                               ***
*** Changed     : 19 Mar 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Fleece is optimized for the fastest Lua to JSON conversion and beats   ***
***  other JSON implementations by around 10 times, native Lua up to 100x.  ***
***                                                                         ***
***  This file holds most of the basis for the higher performance. It does  ***
***  well for non-escaped strings with or without assembler. If you use     ***
***  escaping, ASM *can* make a difference. But consider guarding your      ***
***  string contents against special characters yourself in any case and do ***
***  any necessary escaping yourself. In most cases this will prevent much  ***
***  double checking. See the ~/README.md file for escape performance.      ***
***                                                                         ***
***  Assembler is implemented only for x86, using gcc inline/AT&T format.   ***                                                                         ***  Switch assembler on in src/fleece-config.h, #define FLEECE_ASM or use  ***                                                               ***  one of linux-asm, linux-64-asm, macosx-asm.                            ***
***                                                                         ***
***  The non-assembler part or sufficiently fast and optimized for the      ***
***  task. Depending on architecture, they are only around 10 to 50%        ***
***  slower than assembler, mostly due to highly optimized C library        ***
***  memcpy functions. Obviously, when escaping characters, they can not    ***
***  be used alone but string traversing has to take place. strcspn()       ***                                                                         ***  turned out to be no help performancewise.                              ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  This file is INCLUDED into fleece.c currently. It is agnostic to       ***
***  anything fleece-specific. It's optimized for exactly the JSON escapes  ***
***  " \ / \b \t \n \f \r all other 0-31 and 127. http://www.json.org/      ***
***                                                                         ***
***-------------------------------------------------------------------------**/

/*
*  There are two different approaches to copying when it comes to escape
*  characters, like " and \. One is to first count how many extra bytes
*  to spell these out we will need: each \ adds 1, each \uNNNN adds 5.
*  Or, we try copying and counting at the same time. If we can't be sure
*  to have enough buffer space left, we stop copying - but continue 
*  counting, then allocate enough buffer space and do the copying again.
*
*  See comments in fleece-stringify.c.
*
*  blind copying and escaping of all ctrls, the buffer is assured to suffice:
*
*  This can be done when it's sure that at least 6 times the space of the 
*  copy source is available in the buffer. For usual keys and short values,
*  this will be the case in 95%+ of cases with ordinary buffer sizes of
*  10K - 100K. The blindness allows for skipping the test for sufficient
*  remaining buffer space.
*  
*  mar 17 - hd
*/

/*****************************************************************************\
***                                                                         ***
***                             DECLARATIONS                                ***
***                                                                         ***
\*****************************************************************************/
// TODO: clean up

char *memcpy_escaped_2(char *dst, const char *src, size_t len, size_t bufsize);
char *memcpy_escaped_3(char *dst, const char *src, size_t len, size_t bufsize);
char *memcpy_escaped_4(char *dst, const char *src, size_t len, size_t bufsize);
size_t count_escaped_2(insp_ctrl *ctrl, const char *s, size_t len);
size_t count_escaped_3(insp_ctrl *ctrl, const char *s, size_t len);
size_t count_escaped_4(insp_ctrl *ctrl, const char *s, size_t len);
void purge_escaped(char *s, size_t len);
void asm_purgecpy(char *src, const char *dest, size_t len);
int memcntcpy_escaped_2(char *dst, const char *src, size_t len, size_t bufsize);
int memcntcpy_escaped_3(char *dst, const char *src, size_t len, size_t bufsize);
int memcntcpy_escaped_4(char *dst, const char *src, size_t len, size_t bufsize);

/*                                      ***
*                                      ****
*                                     *****
*                                    * ****
*                                      ****
*                                      ****
*                                      ****
*                                   Handling E1                              */

/*****************************************************************************\
***                                                                         ***
***                            PURGE COPY / E1                              ***
***                                                                         ***
\*****************************************************************************/

/** copy a string and replace \ and " by * on the fly
*
*   This is the fastest way to defuse the danger from " and \. The buffer length
*   is known from the beginning and there is no need for the said "approaches".
*   The replacement char is simply copied over the offending onces.
*   TODO: Instead of '*' set any char in src/fleece-config.h, FLEECE_PURGE_CHAR
*
*   mar 11 - hd
*/

#ifndef FLEECE_ASM
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

#define purge_escaped(___s, ___len) { \
\
	char *p = (___s) -1; \
	char *exend = (___s) + (___len); \
	while(++p < exend) { \
		switch(*p) { case '"': case '\\': *p = FLEECE_PURGE_CHAR; } \
	} \
}

/************************************** ASM **********************************/
#else 
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

#define asm_purgecpy(dst, src, len) {                     \
\
    if(!(!dst || !src || !len)) {                         \
\
    int dummy0, dummy1, dummy2, dummy3;                   \
    __asm__ __volatile__(                                 \
    					   "1:\n\t"                       \
    					   "lodsb\n\t"                    \
    					   "cmp $34, %%al\n\t"            \
    					   "jz purge%=\n\t"               \
    					   "cmp $92, %%al\n\t"            \
    					   "jz purge%=\n\t"               \
    					   "jmp cnt%=\n\t"                \
    					   "purge%=: mov $42, %%al\n\t"   \
	                       "cnt%=:\tstosb\n\t"            \
                           "dec %%cx\n\t"                 \
                           "jne 1b\n\t"                   \
                         : "=&S" (dummy0), "=&D" (dummy1), "=&a" (dummy2), "=&c" (dummy3)                         \
                         : "0" (src),"1" (dst), "3" (len) \
                         : "memory", "cc");               \
    } \
}
#endif // < ASM

/*                                  *******
*                                  ***   ***
*                                  ***   ***
*                                      ****
*                                     ****    
*                                   ****  ***
*                                 ***********
*                                 Handling E2                                */

/*****************************************************************************\
***                                                                         ***
***                           COUNTING FOR E2                               ***
***                                                                         ***
\*****************************************************************************/

/** Count extra bytes needed for escaping, respecting only  " and \
*   E.g. every \ needs 1, every \uNNNN needs 5 extra bytes.
*
*   mar 11 - hd
*/

#ifndef FLEECE_ASM
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

size_t count_escaped_2(insp_ctrl *ctrl, const char *s, size_t len)
{
	size_t char_count = 0;
	size_t byte_count = 0;
	char *p = (char *)s -1;
	char *ex = (char *)s + len;
	while(++p < ex) {
		if(*p == '\\' || *p == '\"') {
			char_count++; byte_count++;
		}
	}

	return byte_count;
}

#else
/************************************** ASM **********************************/

#endif 

/*****************************************************************************\
***                                                                         ***
***                             COPYING FOR E2                              ***
***                                                                         ***
\*****************************************************************************/

#ifndef FLEECE_ASM
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

char *memcpy_escaped_2(char *dst, const char *src, size_t len, size_t bufsize)
{

	char *p = (char *)src;
	char *q = dst;
	char *ex = (char *)src + len;
	char *mex = dst + bufsize;
	while(p < ex) {

		{
			char c = *p++;
			switch (c) {
				case '"' : *q++='\\'; *q++='"'; break;
				case '\\': *q++='\\'; *q++='\\'; break;
				default: *q++ = c;
			}
		}
	}
	
	if(p != ex) { 
		printf(" ** fleece: memcpy_escaped_2 died untimely (1) %p %p (%zd, %zd) ", p,  ex, len, bufsize); 
		printf("src: %p '%.*s' -> '%.*s'\n", src, (int)len, src, (int)bufsize, dst); 
		exit(185); 
	}
	if(q != mex) { 
		printf(" ** fleece: memcpy_escaped_2 died untimely (2) %p %p (%zd, %zd) ", q, mex, len, bufsize); 
		printf("src: %p '%.*s' -> '%.*s'\n", src, (int)len, src, (int)bufsize, dst); 
		exit(186); 
	}
	return q;
}

/************************************** ASM **********************************/

#else // ASM
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

#define memcpy_escaped_2(dst, src, len, bufsize) { \
\
    int inserts; \
    int dummy0, dummy1, dummy2, dummy3; \
    __asm__ __volatile__(  \
    					   "1:\tlodsb\n\t" \
    					   "cmp $34, %%al\n\t" \
    					   "jz escape%=\n\t" \
    					   "cmp $92, %%al\n\t" \
    					   "jz escape%=\n\t" \
	                       "stosb\n\t" \
                           "loopcnt%=:\tdec %%cx\n\t" \
                           "jne 1b\n\t" \
                           "jmp end%=\n\t" \
    					   "escape%=: mov %%al, %%ah\n\t" \
    					   "mov $92, %%al\n\t" \
	                       "stosw\n\t" \
	                       "inc %3\n\t" \
                           "jmp loopcnt%=\n\t" \
                           "end%=:" \
                         : "=&S" (dummy0), "=&D" (dummy1), "=&a" (dummy2), "=&r" (inserts), "=&c" (dummy3) \
                         : "0" (src),"1" (dst), "4" (len), "3" (0) \
                         : "memory", "cc"); \
\
/*
	if(inserts != bufsize - len) { 
		printf(" ** fleece: memcpy_escaped_2 got wrong number of inserts: %d, expected %zd (%zd - %zd) ",
			inserts, bufsize - len, bufsize,  len); 
		printf("src: %p '%.*s' -> '%.*s'\n", src, (int)len, src, (int)bufsize, dst); 
		exit(178); 
	} */ \
\
	/* return dst; */ \
}
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

#define strcpy_escaped_2(dst, src, len, sum) { \
\
    char *dummy0, *dummy1, *dummy2; \
    __asm__ __volatile__(  \
    					   "1:\tlodsb\n\t" \
                           /*----------*/ \
    					   "cmp $92, %%al\n\t" \
    					   "jbe 3f\n\t" \
                           /*----------*/ \
	                       "stosb\n\t" \
                           "jmp 1b\n\t" \
                           /*----------*/ \
    					   "3: cmp $0, %%al\n\t" \
    					   "je end%=\n\t" \
    					   "cmp $34, %%al\n\t" \
    					   "jz escape%=\n\t" \
    					   "cmp $92, %%al\n\t" \
    					   "jz escape%=\n\t" \
                           /*----------*/ \
	                       "stosb\n\t" \
                           "jmp 1b\n\t" \
                           /*----------*/ \
    					   "escape%=:\n\t" \
    					   "mov %%al, %%ah\n\t" \
    					   "mov $92, %%al\n\t" \
	                       "stosw\n\t" \
                           "jmp 1b\n\t" \
                           /*----------*/ \
                           "end%=:" \
                         : "=&S" (dummy0), "=&D" (dummy1), "=&a" (dummy2) \
                         : "0" (src),"1" (dst) \
                         : "memory", "cc"); \
\
    sum = (size_t)(dummy1 - dst); \
\
}

/*****************************************************************************\
***                                                                         ***
***                     COUNTING WHILE COPYING FOR E2                       ***
***                                                                         ***
\*****************************************************************************/

/** simultaneous counting, copying and escaping " and \,  when getting too
*   long, cancel copying and escaping but continue counting needed extra bytes.
*
*   1. copy a string and escape all ctrl and special ASCII characters on the fly 
*   2. count the extra bytes need for escaping on the fly
*   3. stop copying when more than N extra bytes are used up but continue count
*
*   this is used to do it all in one when possible but revert to only counting
*   when it turns out that more space will be needed. Then, at least that extra
*   space's size is found out by traversing the source string to the end. The
*   destination then simply stays incomplete.
*
*   TODO: implement for non-asm
*
*   mar 11 - hd
*/

/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

#define memcntcpy_escaped_2(dst, src, len, bufsize) { \
\
    int inserts; \
    int dummy0, dummy1, dummy2, dummy3; \
    __asm__ __volatile__(  \
    					   "1:\tlodsb\n\t" \
    					   "cmp $34, %%al\n\t" \
    					   "jz escape2cc%=\n\t" \
    					   "cmp $92, %%al\n\t" \
    					   "jz escape2cc%=\n\t" \
    					   "cmp $" RSRV_BUF ", %3\n\t" \
    					   "ja loopcnt2cc%=\n\t" \
	                       "stosb\n\t" \
                           "loopcnt2cc%=:\n\t" \
                           "dec %%cx\n\t" \
                           "jne 1b\n\t" \
                           "jmp end2cc%=\n\t" \
    					   /*----------*/ \
    					   "escape2cc%=:\n\t" \
    					   "inc %3\n\t" \
    					   "cmp $" RSRV_BUF ", %3\n\t" \
    					   "ja loopcnt2cc%=\n\t" \
    					   "mov %%al, %%ah\n\t" \
    					   "mov $92, %%al\n\t" \
	                       "stosw\n\t" \
                           "jmp loopcnt2cc%=\n\t" \
                           "end2cc%=:" \
                         : "=&S" (dummy0), "=&D" (dummy1), "=&a" (dummy2), "=&r" (inserts), "=&c" (dummy3) \
                         : "0" (src),"1" (dst), "4" (len), "3" (0) \
                         : "memory", "cc"); \
\
    needs = inserts; \
	\
}
#endif // ASM

/*                                  *******
*                                  ***   ***
*                                        ***
*                                     *****
*                                        ****    
*                                 ***    ****  
*                                   ******* 
*/
/*****************************************************************************\
***                                                                         ***
***                           COUNTING FOR E3                               ***
***                                                                         ***
\*****************************************************************************/

/** Count extra bytes needed for escaping, respecting only  ", \ and /
*   E.g. every \ needs 1, every \uNNNN needs 5 extra bytes.
*
*   The assembler version can be used for gcc on x86 processors by
*   defining FLEECE_ASM in src/fleece-config.h
*
*   mar 11 - hd
*/
#ifndef FLEECE_ASM
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

size_t count_escaped_3(insp_ctrl *ctrl, const char *s, size_t len)
{
	size_t char_count = 0;
	size_t byte_count = 0;
	char *p = (char *)s -1;
	char *ex = (char *)s + len;
	while(++p < ex) {
		if(*p == '\\' || *p == '\"' || *p == '/') {
			char_count++; byte_count++; 
		}
	}

	return byte_count;
}

#else // ASM:

#endif

/*****************************************************************************\
***                                                                         ***
***                             COPYING FOR E3                              ***
***                                                                         ***
\*****************************************************************************/

#ifndef FLEECE_ASM
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

char *memcpy_escaped_3(char *dst, const char *src, size_t len, size_t bufsize)
{

	char *p = (char *)src;
	char *q = dst;
	char *ex = (char *)src + len;
	#if SELFCHECK >= 2
	char *mex = dst + bufsize;
    #endif
	while(p < ex) {
		
		{
			char c = *p++;
				switch (c) {
					case '"' : *q++='\\'; *q++='"'; break;
					case '\\': *q++='\\'; *q++='\\'; break;
					case '/' : *q++='\\'; *q++='/'; break;
					default: *q++ = c;
				}
		}
	}
	
	#if SELFCHECK >= 2
	if(p != ex) { 
		printf(" ** fleece: memcpy_escaped_3 died untimely (1) %p %p (%zd, %zd) ", p,  ex, len, bufsize); 
		printf("src: %p '%.*s' -> '%.*s'\n", src, (int)len, src, (int)bufsize, dst); 
		exit(183); 
	}
	if(q != mex) { 
		printf(" ** fleece: memcpy_escaped_3 died untimely (2) %p %p (%zd, %zd) ", q, mex, len, bufsize); 
		printf("src: %p '%.*s' -> '%.*s'\n", src, (int)len, src, (int)bufsize, dst); 
		exit(184); 
	}
	#endif
	return q;
}

/************************************** ASM **********************************/

#else // ASM:
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

#define memcpy_escaped_3(dst, src, len, bufsize) { \
\
    int inserts; \
    int dummy0, dummy1, dummy2, dummy3; \
    __asm__ __volatile__(  \
    					   "1:\tlodsb\n\t" \
    					   "cmp $34, %%al\n\t" \
    					   "jz escape3%=\n\t" \
    					   "cmp $47, %%al\n\t" \
    					   "jz escape3%=\n\t" \
    					   "cmp $92, %%al\n\t" \
    					   "jz escape3%=\n\t" \
	                       "stosb\n\t" \
                           "loopcnt3%=:\tdec %%cx\n\t" \
                           "jne 1b\n\t" \
                           "jmp end3%=\n\t" \
    					   "escape3%=: mov %%al, %%ah\n\t" \
    					   "mov $92, %%al\n\t" \
	                       "stosw\n\t" \
	                       "inc %3\n\t" \
                           "jmp loopcnt3%=\n\t" \
                           "end3%=:" \
                         : "=&S" (dummy0), "=&D" (dummy1), "=&a" (dummy2), "=&r" (inserts), "=&c" (dummy3) \
                         : "0" (src),"1" (dst), "4" (len), "3" (0) \
                         : "memory", "cc"); \
 \
	\
}

/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

#define strcpy_escaped_3(dst, src, len, sum) { \
\
    char *dummy0, *dummy1, *dummy2; \
    __asm__ __volatile__(  \
    					   "1:\tlodsb\n\t" \
    					   "cmp $0, %%al\n\t" \
    					   "je end3%=\n\t" \
    					   "cmp $34, %%al\n\t" \
    					   "jz escape3%=\n\t" \
    					   "cmp $47, %%al\n\t" \
    					   "jz escape3%=\n\t" \
    					   "cmp $92, %%al\n\t" \
    					   "jz escape3%=\n\t" \
	                       "stosb\n\t" \
                           "jmp 1b\n\t" \
    					   "escape3%=: mov %%al, %%ah\n\t" \
    					   "mov $92, %%al\n\t" \
	                       "stosw\n\t" \
                           "jmp 1b\n\t" \
                           "end3%=:" \
                         : "=&S" (dummy0), "=&D" (dummy1), "=&a" (dummy2) \
                         : "0" (src),"1" (dst) \
                         : "memory", "cc"); \
\
    sum = (size_t)(dummy1 - dst); \
\
}

/*****************************************************************************\
***                                                                         ***
***                      COUNTING WITH COPYING FOR E3                       ***
***                                                                         ***
\*****************************************************************************/

/** simultaneous counting, copying and escaping ", \ and /,  when getting too
*   long, cancel copying and escaping but continue counting needed extra bytes.
*
*   1. copy a string and escape all ctrl and special ASCII characters on the fly 
*   2. count the extra bytes need for escaping on the fly
*   3. stop copying when more than N extra bytes are used up but continue count
*
*   this is used to do it all in one when possible but revert to only counting
*   when it turns out that more space will be needed. Then, at least that extra
*   space's size is found out by traversing the source string to the end. The
*   destination then simply stays incomplete.
*
*   mar 11 - hd
*/

/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

#define memcntcpy_escaped_3(dst, src, len, bufsize) { \
\
    int inserts; \
    int dummy0, dummy1, dummy2, dummy3; \
    __asm__ __volatile__(  \
    					   "1:\tlodsb\n\t" \
    					   "cmp $34, %%al\n\t" \
    					   "jz escape3cc%=\n\t" \
    					   "cmp $47, %%al\n\t" \
    					   "jz escape3cc%=\n\t" \
    					   "cmp $92, %%al\n\t" \
    					   "jz escape3cc%=\n\t" \
    					   "cmp $" RSRV_BUF ", %3\n\t" \
    					   "ja loopcnt3cc%=\n\t" \
	                       "stosb\n\t" \
                           "loopcnt3cc%=:\n\t" \
                           "dec %%cx\n\t" \
                           "jne 1b\n\t" \
                           "jmp end3cc%=\n\t" \
    					   /*----------*/ \
    					   "escape3cc%=:\n\t" \
    					   "inc %3\n\t" \
    					   "cmp $" RSRV_BUF ", %3\n\t" \
    					   "ja loopcnt3cc%=\n\t" \
    					   "mov %%al, %%ah\n\t" \
    					   "mov $92, %%al\n\t" \
	                       "stosw\n\t" \
                           "jmp loopcnt3cc%=\n\t" \
                           "end3cc%=:" \
                         : "=&S" (dummy0), "=&D" (dummy1), "=&a" (dummy2), "=&r" (inserts), "=&c" (dummy3) \
                         : "0" (src),"1" (dst), "4" (len), "3" (0) \
                         : "memory", "cc"); \
\
    needs = inserts; \
	\
}

#endif // ASM

/*                                       ***
*                                       ***
*                                     ***    *
*                                    ****  ***
*                                   ************
*                                         ****
*                                         ****
*/
/*****************************************************************************\
***                                                                         ***
***                           COUNTING FOR E/E4                             ***
***                                                                         ***
\*****************************************************************************/

#ifndef FLEECE_ASM

/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

size_t count_escaped_4(insp_ctrl *ctrl, const char *s, size_t len)
{
	/*	\" 	\\ 	\/  \b(8) 	\f(12)	\n(10) 	\r(13) 	\t(9) 	\u four-hex-digits 	*/

	size_t char_count = 0;
	size_t byte_count = 0;
	char *p = (char *)s -1;
	char *ex = (char *)s + len;
	while(++p < ex) {
		if(*p>='a' && *p <= 'z') {}
		else if(*p<32) {
			switch (*p) {
			case   8: char_count++; byte_count++; break;
			case   9: char_count++; byte_count++; break;
			case  10: char_count++; byte_count++; break;
			case  12: char_count++; byte_count++; break;
			case  13: char_count++; byte_count++; break;
			default : char_count++; byte_count += 5; break;
			}   
		}
		else {
			switch (*p) {
			case  127 : char_count++; byte_count+= 5; break;
			case  '"' : char_count++; byte_count++; break;
			case  '\\': char_count++; byte_count++; break;
			case  '/' : char_count++; byte_count++; break;
			}
		}
	}
	
	return byte_count;
}

/************************************** *** *********************************/

#else // ASM:

#endif

/*****************************************************************************\
***                                                                         ***
***                             COPYING FOR E4                              ***
***                                                                         ***
\*****************************************************************************/

#ifndef FLEECE_ASM
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

char *memcpy_escaped_4(char *dst, const char *src, size_t len, size_t bufsize)
{
	char hex[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	
	char *p = (char *)src;
	char *q = dst;
	char *ex = (char *)src + len;
	#if SELFCHECK >= 2
	char *mex = dst + bufsize;
    #endif
	while(p < ex) {

		{
			char c = *p++;
			if(c<32) {
				switch (c) {
					case   8 : *q++='\\'; *q++='b'; break;
					case   9 : *q++='\\'; *q++='t'; break;
					case  10 : *q++='\\'; *q++='n'; break;
					case  12 : *q++='\\'; *q++='f'; break;
					case  13 : *q++='\\'; *q++='r'; break;
					default  : *q++='\\'; *q++='u'; *q++='0'; *q++='0'; *q++=hex[c>>4]; *q++=hex[c&0xf];
				}
			} /* > 32: */
			else
			{
				switch (c) {
					case 127 : *q++='\\'; *q++='u'; *q++='0'; *q++='0'; *q++='7'; *q++='f'; break;
					case '"' : *q++='\\'; *q++='"'; break;
					case '\\': *q++='\\'; *q++='\\'; break;
					case '/' : *q++='\\'; *q++='/'; break;
					default: *q++ = c;
					/* default  : { printf(" ** fleece: escape error for char %d (%c)\n", c, c); exit(189); } */
				}
			}
		}
	}
	
	#if SELFCHECK >= 2
	/* q is one after last. so is p, by virtue of while(++p) */
	if(p != ex) { 
		printf(" ** fleece: memcpy_escaped_4 died untimely (1) %p %p (%zd, %zd) ", p,  ex, len, bufsize); 
		printf("src: %p '%.*s' -> '%.*s'\n", src, (int)len, src, (int)bufsize, dst); 
		exit(192); 
	}
	if(q != mex) { 
		printf(" ** fleece: memcpy_escaped_4 died untimely (2) %p %p (%zd, %zd) ", q, mex, len, bufsize); 
		printf("src: %p '%.*s' -> '%.*s'\n", src, (int)len, src, (int)bufsize, dst); 
		exit(191); 
	}
	#endif
	return q;
}

/************************************** ASM **********************************/

#else // ASM:
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

#define memcpy_escaped_4(dst, src, len, bufsize) { \
\
    \
	\
	int dummy0, dummy1, dummy2, dummy3; \
    __asm__ __volatile__(  \
    					   "1:\tlodsb\n\t"          \
    					   "cmp $127, %%al\n\t"     /* tackle 127 */\
    					   "je uctrl%=\n\t"           /* handle 127 */\
    					   "cmp $92, %%al\n\t"      /* 93 .. a - z .. 255 | fallthrough */\
    					   "jbe trynumcpy%=\n\t"      \
                           /* -----------*/         \
                           "stosb\n\t"              /* WRITE UNCHANGED */\
                           "dec %%cx\n\t"           /* loop foot: simple (93 - 255) */\ 
                           "jne 1b\n\t"             \
                           "jmp endallcpy%=\n\t"      \
                           /* -----------*/         \
    					   "trynumcpy%=:\n\t"         \
    					   "je bs92cpy%=\n\t"         /* handle 92 (\) */\
    					   "cmp $47, %%al\n\t"      /* tackle 47 (/) */\
    					   "jbe tryrestcpy%=\n\t"     /* 48 .. 0 - 9 .. A - Z .. 91 | 'fallthrough' */\
                           /* -----------*/         \
                           "stosb\n\t"              /* WRITE UNCHANGED */\
                           "dec %%cx\n\t"           /* loop foot: (48 - 91) */\
                           "jne 1b\n\t"             \
                           "jmp endallcpy%=\n\t"      \
                           /* -----------*/         \
    					   "tryrestcpy%=:\n\t"        \
    					   "je bscpy%=\n\t"           /* handle 47 (/) */\
    					   "cmp $34, %%al\n\t"      /* tackle 34 (") */\
    					   "je bscpy%=\n\t"           /* handle 34 (") */\
    					   "cmp $31, %%al\n\t"      /* tackle 31 and below (ctrls) */\
    					   "jbe lowctrlscpy%=\n\t"    /* handle all remaining ctrls */\
                           /* -----------*/         \
                           "stosb\n\t"              /* WRITE UNCHANGED */\
                           "dec %%cx\n\t"           /* loop foot: (32 - 46) */\
                           "jne 1b\n\t"             \
                           "jmp endallcpy%=\n\t"      \
                           /* -----------*/         \
    					   "lowctrlscpy%=:\n\t"       \
    					   "cmp $9, %%al\n\t"       /* tackle \t */\
    					   "je ctrl9cpy%=\n\t"        \
    					   "cmp $10, %%al\n\t"      /* tackle \r */\
    					   "je ctrl10cpy%=\n\t"       \
    					   "cmp $13, %%al\n\t"      /* tackle \n */\
    					   "je ctrl13cpy%=\n\t"       \
    					   "cmp $12, %%al\n\t"      /* tackle \f */\
    					   "je ctrl12cpy%=\n\t"        \
    					   "\tcmp $8, %%al\n\t"     /* tackle \b */\
    					   "jne uctrl%=\n\t"          /* all 0-31 that are not 8 9 19 12 13 */\
    					   "jmp ctrl8cpy%=\n\t"       /* jump 8 (\b) */\
                           /* -----------*/         \
                           /* -----------*/         \
    					   "bscpy%=:\n\t"             /* handle single \ ctrls */\
						   "mov %%al, %%ah\n\t"     \
                           "mov $92, %%al\n\t"      /* (\) */\
                           "stosw\n\t"              /* write word */\
                           "\tdec %%cx\n\t"         /* loop foot */\ 
                           "jne 1b\n\t"             \
                           "jmp endallcpy%=\n\t"   \
                           /* -----------*/         \
    					   "bs92cpy%=:\n\t"           /* handle single \ ctrls */\
                           "mov $92, %%ah\n\t"      /* (\) */\
                           "stosw\n\t"              /* write word */\
                           "\tdec %%cx\n\t"         /* loop foot */\ 
                           "jne 1b\n\t"             \
                           "jmp endallcpy%=\n\t"   \
                           /* -----------*/         \
    					   "ctrl47cpy%=:\n\t"          /* handle / */\
                           "mov $92, %%al\n\t"      /* (\) */\
                           "stosw\n\t"              /* write word */\
                           "\tdec %%cx\n\t"         /* loop foot */\ 
                           "jne 1b\n\t"             \
                           "jmp endallcpy%=\n\t"   \
                           /* -----------*/         \
    					   "ctrl8cpy%=:\n\t"          /* handle \b */\
                           "mov $0x625c,%%ax\n\t"   /* ('\' 'b') */\
                           "stosw\n\t"              /* write word */\
                           "\tdec %%cx\n\t"         /* loop foot */\ 
                           "jne 1b\n\t"             \
                           "jmp endallcpy%=\n\t"   \
                           /* -----------*/         \
    					   "ctrl9cpy%=:\n\t"          /* handle \t */\
                           "mov $0x745c,%%ax\n\t"   /* ('\' 'b') */\
                           "stosw\n\t"              /* write word */\
                           "\tdec %%cx\n\t"         /* loop foot */\ 
                           "jne 1b\n\t"             \
                           "jmp endallcpy%=\n\t"   \
                           /* -----------*/         \
    					   "ctrl10cpy%=:\n\t"         /* handle \n */\
                           "mov $0x6e5c,%%ax\n\t"   /* ('\' 'b') */\
                           "stosw\n\t"              /* write word */\
                           "\tdec %%cx\n\t"         /* loop foot */\ 
                           "jne 1b\n\t"             \
                           "jmp endallcpy%=\n\t"   \
                           /* -----------*/         \
    					   "ctrl12cpy%=:\n\t"         /* handle \f */\
                           "mov $0x665c,%%ax\n\t"   /* ('\' 'b') */\
                           "stosw\n\t"              /* write word */\
                           "\tdec %%cx\n\t"         /* loop foot */\ 
                           "jne 1b\n\t"             \
                           "jmp endallcpy%=\n\t"   \
                           /* -----------*/         \
    					   "ctrl13cpy%=:\n\t"          /* handle \r */\
                           "mov $0x725c,%%ax\n\t"   /* ('\' 'b') */\
                           "stosw\n\t"              /* write word */\
                           "\tdec %%cx\n\t"         /* loop foot */\ 
                           "jne 1b\n\t"             \
                           "jmp endallcpy%=\n\t"   \
                           /* -----------*/         \
    					   "uctrl%=:\n\t"             /* all 0-31 that are not 8 9 19 12 13 */\
                           "mov %%al, %%ah\n\t"     /* 'push' */  \
                           "mov $92, %%al\n\t"      /* (\) */  \
                           "stosb\n\t"              /* write */\
                           "mov $117, %%al\n\t"     /* (u) */  \
                           "stosb\n\t"              /* write */\
                           "mov $48, %%al\n\t"      /* (0) */  \
                           "stosb\n\t"              /* write */\
                           "mov $48, %%al\n\t"      /* (0) */  \
                           "stosb\n\t"              /* write */\
                           "mov %%ah, %%al\n\t"     /* 'pop' */\
                           "shr $4, %%al\n\t"       /* (0)+x */\
                           "add $48, %%al\n\t"      /* (0)+x */\
                           "stosb\n\t"              /* write */\
                           "mov %%ah, %%al\n\t"     /* 'pop' */\
                           "and $0xf, %%al\n\t"     /* (0)+x */\
                           "cmp $0xa, %%al\n\t"     /* (0)+x */\
                           "jb arab%=\n\t"            /* (0)+x */\
                           "add $39, %%al\n\t"      /* (0)+x */\
                           "arab%=:\tadd $48, %%al\n\t"/* (0)+x */\
                           "stosb\n\t"              /* write */\
                           /* -----------*/         \
                           "loopfootctrl%=:\n\t"      \
                           "\tdec %%cx\n\t"         /* loop foot (all ctrls) */\ 
                           "jne 1b\n\t"             \
                           /* -----------*/         \
                           "endallcpy%=:"             \
                         : "=&S" (dummy0), "=&D" (dummy1), "=&a" (dummy2), "=&c"  (dummy3), "=&d" (dummy3) \
                         : "0" (src), "1" (dst), "3" (len) \
                         : "memory", "cc"); \
\
}

/* strpy stops when hitting 0, faster as it makes one counter dispensible. */
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

#define strcpy_escaped_4(dst, src, len, sum) { \
\
	char *dummy0, *dummy1, *dummy2; \
    __asm__ __volatile__(  \
    					   "1:\tlodsb\n\t"          \
    					   "cmp $0, %%al\n\t"     /* stop copying */\
                           "je endallcpy%=\n\t"      \
    					   "cmp $127, %%al\n\t"     /* tackle 127 */\
    					   "je uctrl%=\n\t"           /* handle 127 */\
    					   "cmp $92, %%al\n\t"      /* 93 .. a - z .. 255 | fallthrough */\
    					   "jbe trynumcpy%=\n\t"      \
                           /* -----------*/         \
                           "stosb\n\t"              /* WRITE UNCHANGED */\
                           "jmp 1b\n\t"             \
                           /* -----------*/         \
    					   "trynumcpy%=:\n\t"         \
    					   "je bs92cpy%=\n\t"         /* handle 92 (\) */\
    					   "cmp $47, %%al\n\t"      /* tackle 47 (/) */\
    					   "jbe tryrestcpy%=\n\t"     /* 48 .. 0 - 9 .. A - Z .. 91 | 'fallthrough' */\
                           /* -----------*/         \
                           "stosb\n\t"              /* WRITE UNCHANGED */\
                           "jmp 1b\n\t"             \
                           /* -----------*/         \
    					   "tryrestcpy%=:\n\t"        \
    					   "je bscpy%=\n\t"           /* handle 47 (/) */\
    					   "cmp $34, %%al\n\t"      /* tackle 34 (") */\
    					   "je bscpy%=\n\t"           /* handle 34 (") */\
    					   "cmp $31, %%al\n\t"      /* tackle 31 and below (ctrls) */\
    					   "jbe lowctrlscpy%=\n\t"    /* handle all remaining ctrls */\
                           /* -----------*/         \
                           "stosb\n\t"              /* WRITE UNCHANGED */\
                           "jmp 1b\n\t"             \
                           /* -----------*/         \
    					   "lowctrlscpy%=:\n\t"       \
    					   "cmp $9, %%al\n\t"       /* tackle \t */\
    					   "je ctrl9cpy%=\n\t"        \
    					   "cmp $10, %%al\n\t"      /* tackle \r */\
    					   "je ctrl10cpy%=\n\t"       \
    					   "cmp $13, %%al\n\t"      /* tackle \n */\
    					   "je ctrl13cpy%=\n\t"       \
    					   "cmp $12, %%al\n\t"      /* tackle \f */\
    					   "je ctrl12cpy%=\n\t"        \
    					   "\tcmp $8, %%al\n\t"     /* tackle \b */\
    					   "jne uctrl%=\n\t"          /* all 0-31 that are not 8 9 19 12 13 */\
    					   "jmp ctrl8cpy%=\n\t"       /* jump 8 (\b) */\
                           /* -----------*/         \
                           /* -----------*/         \
    					   "bscpy%=:\n\t"             /* handle single \ ctrls */\
						   "mov %%al, %%ah\n\t"     \
                           "mov $92, %%al\n\t"      /* (\) */\
                           "stosw\n\t"              /* write word */\
                           "jmp 1b\n\t"             \
                           /* -----------*/         \
    					   "bs92cpy%=:\n\t"           /* handle single \ ctrls */\
                           "mov $92, %%ah\n\t"      /* (\) */\
                           "stosw\n\t"              /* write word */\
                           "jmp 1b\n\t"             \
                           /* -----------*/         \
    					   "ctrl47cpy%=:\n\t"          /* handle / */\
                           "mov $92, %%al\n\t"      /* (\) */\
                           "stosw\n\t"              /* write word */\
                           "jmp 1b\n\t"             \
                           /* -----------*/         \
    					   "ctrl8cpy%=:\n\t"          /* handle \b */\
                           "mov $0x625c,%%ax\n\t"   /* ('\' 'b') */\
                           "stosw\n\t"              /* write word */\
                           "jmp 1b\n\t"             \
                           /* -----------*/         \
    					   "ctrl9cpy%=:\n\t"          /* handle \t */\
                           "mov $0x745c,%%ax\n\t"   /* ('\' 'b') */\
                           "stosw\n\t"              /* write word */\
                           "jmp 1b\n\t"             \
                           /* -----------*/         \
    					   "ctrl10cpy%=:\n\t"         /* handle \n */\
                           "mov $0x6e5c,%%ax\n\t"   /* ('\' 'b') */\
                           "stosw\n\t"              /* write word */\
                           "jmp 1b\n\t"             \
                           /* -----------*/         \
    					   "ctrl12cpy%=:\n\t"         /* handle \f */\
                           "mov $0x665c,%%ax\n\t"   /* ('\' 'b') */\
                           "stosw\n\t"              /* write word */\
                           "jmp 1b\n\t"             \
                           /* -----------*/         \
    					   "ctrl13cpy%=:\n\t"          /* handle \r */\
                           "mov $0x725c,%%ax\n\t"   /* ('\' 'b') */\
                           "stosw\n\t"              /* write word */\
                           "jmp 1b\n\t"             \
                           /* -----------*/         \
    					   "uctrl%=:\n\t"             /* all 0-31 that are not 8 9 19 12 13 */\
                           "mov %%al, %%ah\n\t"     /* 'push' */  \
                           "mov $92, %%al\n\t"      /* (\) */  \
                           "stosb\n\t"              /* write */\
                           "mov $117, %%al\n\t"     /* (u) */  \
                           "stosb\n\t"              /* write */\
                           "mov $48, %%al\n\t"      /* (0) */  \
                           "stosb\n\t"              /* write */\
                           "mov $48, %%al\n\t"      /* (0) */  \
                           "stosb\n\t"              /* write */\
                           "mov %%ah, %%al\n\t"     /* 'pop' */\
                           "shr $4, %%al\n\t"       /* (0)+x */\
                           "add $48, %%al\n\t"      /* (0)+x */\
                           "stosb\n\t"              /* write */\
                           "mov %%ah, %%al\n\t"     /* 'pop' */\
                           "and $0xf, %%al\n\t"     /* (0)+x */\
                           "cmp $0xa, %%al\n\t"     /* (0)+x */\
                           "jb arab%=\n\t"            /* (0)+x */\
                           "add $39, %%al\n\t"      /* (0)+x */\
                           "arab%=:\tadd $48, %%al\n\t"/* (0)+x */\
                           "stosb\n\t"              /* write */\
                           "jmp 1b\n\t"             \
                           /* -----------*/         \
                           "endallcpy%=:"             \
                         : "=&S" (dummy0), "=&D" (dummy1), "=&a" (dummy2) \
                         : "0" (src), "1" (dst) \
                         : "memory", "cc"); \
\
    sum = (size_t)(dummy1 - dst); \
\
}

/*****************************************************************************\
***                                                                         ***
***                      COUNTING WITH COPYING FOR E4                       ***
***                                                                         ***
\*****************************************************************************/

/** simultaneous counting, copying and escaping of all ctrls, when getting too
*   long, cancel copying and escaping but continue counting needed extra bytes.
*
*   1. copy a string and escape all ctrl and special ASCII characters on the fly 
*   2. count the extra bytes need for escaping on the fly
*   3. stop copying when more than N extra bytes are used up but continue count
*
*   this is used to do it all in one when possible but revert to only counting
*   when it turns out that more space will be needed. Then, at least that extra
*   space's size is found out by traversing the source string to the end. The
*   destination then simply stays incomplete.
*
*   mar 11 - hd
*/
/* sdfsdf /*/
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/

#define memcntcpy_escaped_4(dst, src, len, bufsize) { \
\
    \
    \
\
	int inserts = 0; \
	int dummy0, dummy1, dummy2, dummy3; \
    __asm__ __volatile__(  \
                   "1:\tlodsb\n\t"          \
                   "cmp $127, %%al\n\t"     /* tackle 127 */\
                   "je uctrl4cc%=\n\t"        /* handle 127 */\
                   "cmp $92, %%al\n\t"      /* 93..a-z..255 fallthrough */\
                   "jbe trynum4cc%=\n\t"      \
                   /* -----------*/         \
                   "cmp $" RSRV_BUF ", %%dx\n\t"       \
                   "ja skip14cc%=\n\t"        \
                   "stosb\n\t"              /* WRITE UNCHANGED */\
                   "skip14cc%=:\tdec %%cx\n\t"           /* loop foot: simple (93 - 255) */\ 
                   "jne 1b\n\t"             \
                   "jmp endall4cc%=\n\t"      \
                   /* -----------*/         \
                   "trynum4cc%=:\n\t"         \
                   "cmp $92, %%al\n\t"      /* */\
                   "je bs92cpy4cc%=\n\t"         /* handle 92 (\) */\
                   "cmp $47, %%al\n\t"      /* tackle 47 (/) */\
                   "jbe tryrest4cc%=\n\t"     /* 48 .. 0 - 9 .. A - Z .. 91 | 'fallthrough' */\
                   /* -----------*/         \
                   "cmp $" RSRV_BUF ", %%dx\n\t"       \
                   "ja skip24cc%=\n\t"        \
                   "stosb\n\t"              /* WRITE UNCHANGED */\
                   "skip24cc%=:\tdec %%cx\n\t"           /* loop foot: (48 - 91) */\
                   "jne 1b\n\t"             \
                   "jmp endall4cc%=\n\t"      \
                   /* -----------*/         \
                   "tryrest4cc%=:\n\t"        \
                   "je bscpy4cc%=\n\t"        /* handle 47 (/) */\
                   "cmp $34, %%al\n\t"      /* tackle 34 (") */\
                   "je bscpy4cc%=\n\t"        /* handle 34 (") */\
                   "cmp $31, %%al\n\t"      /* tackle 31 and below (ctrls) */\
                   "jbe lowctrl4cc%=\n\t"     /* handle all remaining ctrls */\
                   /* -----------*/         \
                   "cmp $" RSRV_BUF ", %%dx\n\t"       \
                   "ja skip34cc%=\n\t"        \
                   "stosb\n\t"              /* WRITE UNCHANGED */\
                   "skip34cc%=:\tdec %%cx\n\t"           /* loop foot: (32 - 46) */\
                   "jne 1b\n\t"             \
                   "jmp endall4cc%=\n\t"      \
                   /* -----------*/         \
                   "lowctrl4cc%=:\n\t"        \
                   "cmp $9, %%al\n\t"       /* tackle \t */\
                   "je ctrl9cpy4cc%=\n\t"     \
                   "cmp $10, %%al\n\t"      /* tackle \r */\
                   "je ctrl10cpy4cc%=\n\t"    \
                   "cmp $13, %%al\n\t"      /* tackle \n */\
                   "je ctrl13cpy4cc%=\n\t"    \
                   "cmp $12, %%al\n\t"      /* tackle \f */\
                   "je ctrl12cpy4cc%=\n\t"    \
                   "cmp $8, %%al\n\t"       /* tackle \b */\
                   "jne uctrl4cc%=\n\t"       /* all 0-31 that are not 8 9 19 12 13 */\
                   "jmp ctrl8cpy4cc%=\n\t"    /* jump 8 (\b) */\
                   /* -----------*/         \
                   /* -----------*/         \
                   "bscpy4cc%=:\n\t"             /* handle single \ ctrls */\
                   "inc %%dx\n\t" \
                   "cmp $" RSRV_BUF ", %%dx\n\t" \
                   "ja loopfootctrl4cc%=\n\t" \
                   "mov %%al, %%ah\n\t"     \
                   "mov $92, %%al\n\t"      /* (\) */\
                   "stosw\n\t"              /* write word */\
                   "dec %%cx\n\t"         /* loop foot */\ 
                   "jne 1b\n\t"      \
                   "jmp endall4cc%=\n\t"   \
                   /* -----------*/         \
                   "bs92cpy4cc%=:\n\t"           /* handle single \ ctrls */\
                   "inc %%dx\n\t" \
                   "cmp $" RSRV_BUF ", %%dx\n\t" \
                   "ja loopfootctrl4cc%=\n\t" \
                   "mov $92, %%ah\n\t"      /* (\) */\
                   "stosw\n\t"              /* write word */\
                   "dec %%cx\n\t"         /* loop foot */\ 
                   "jne 1b\n\t"      \
                   "jmp endall4cc%=\n\t"   \
                   /* -----------*/         \
                   "ctrl47cpy4cc%=:\n\t"          /* handle / */\
                   "inc %%dx\n\t" \
                   "cmp $" RSRV_BUF ", %%dx\n\t" \
                   "ja loopfootctrl4cc%=\n\t" \
                   "mov $92, %%al\n\t"      /* (\) */\
                   "stosw\n\t"              /* write word */\
                   "dec %%cx\n\t"         /* loop foot */\ 
                   "jne 1b\n\t"      \
                   "jmp endall4cc%=\n\t"   \
                   /* -----------*/         \
                   "ctrl8cpy4cc%=:\n\t"          /* handle \b */\
                   "inc %%dx\n\t" \
                   "cmp $" RSRV_BUF ", %%dx\n\t" \
                   "ja loopfootctrl4cc%=\n\t" \
                   "mov $0x625c,%%ax\n\t"   /* ('\' 'b') */\
                   "stosw\n\t"              /* write word */\
                   "dec %%cx\n\t"         /* loop foot */\ 
                   "jne 1b\n\t"      \
                   "jmp endall4cc%=\n\t"   \
                   /* -----------*/         \
                   "ctrl9cpy4cc%=:\n\t"          /* handle \t */\
                   "inc %%dx\n\t" \
                   "cmp $" RSRV_BUF ", %%dx\n\t" \
                   "ja loopfootctrl4cc%=\n\t" \
                   "mov $0x745c,%%ax\n\t"   /* ('\' 'b') */\
                   "stosw\n\t"              /* write word */\
                   "dec %%cx\n\t"         /* loop foot */\ 
                   "jne 1b\n\t"      \
                   "jmp endall4cc%=\n\t"   \
                   /* -----------*/         \
                   "ctrl10cpy4cc%=:\n\t"         /* handle \n */\
                   "inc %%dx\n\t" \
                   "cmp $" RSRV_BUF ", %%dx\n\t" \
                   "ja loopfootctrl4cc%=\n\t" \
                   "mov $0x6e5c,%%ax\n\t"   /* ('\' 'b') */\
                   "stosw\n\t"              /* write word */\
                   "dec %%cx\n\t"         /* loop foot */\ 
                   "jne 1b\n\t"      \
                   "jmp endall4cc%=\n\t"   \
                   /* -----------*/         \
                   "ctrl12cpy4cc%=:\n\t"         /* handle \f */\
                   "inc %%dx\n\t" \
                   "cmp $" RSRV_BUF ", %%dx\n\t" \
                   "ja loopfootctrl4cc%=\n\t" \
                   "mov $0x665c,%%ax\n\t"   /* ('\' 'b') */\
                   "stosw\n\t"              /* write word */\
                   "dec %%cx\n\t"         /* loop foot */\ 
                   "jne 1b\n\t"      \
                   "jmp endall4cc%=\n\t"   \
                   /* -----------*/         \
                   "ctrl13cpy4cc%=:\n\t"          /* handle \r */\
                   "inc %%dx\n\t" \
                   "cmp $" RSRV_BUF ", %%dx\n\t" \
                   "ja loopfootctrl4cc%=\n\t" \
                   "mov $0x725c,%%ax\n\t"   /* ('\' 'b') */\
                   "stosw\n\t"              /* write word */\
                   "dec %%cx\n\t"         /* loop foot */\ 
                   "jne 1b\n\t"      \
                   "jmp endall4cc%=\n\t"   \
                   /* -----------*/         \
                   /* -----------*/         \
                   "uctrl4cc%=:\n\t"        /* all 0-31 that are not 8 9 19 12 13 */\
                   "add $5, %%dx\n\t"       \
                   "cmp $" RSRV_BUF ", %%dx\n\t" \
                   "ja loopfootctrl4cc%=\n\t" \
                           "mov %%al, %%ah\n\t"     /* 'push' */  \
                           "mov $92, %%al\n\t"      /* (\) */  \
                           "stosb\n\t"              /* write */\
                           "mov $117, %%al\n\t"     /* (u) */  \
                           "stosb\n\t"              /* write */\
                           "mov $48, %%al\n\t"      /* (0) */  \
                           "stosb\n\t"              /* write */\
                           "mov $48, %%al\n\t"      /* (0) */  \
                           "stosb\n\t"              /* write */\
                           "mov %%ah, %%al\n\t"     /* 'pop' */\
                           "shr $4, %%al\n\t"       /* (0)+x */\
                           "add $48, %%al\n\t"      /* (0)+x */\
                           "stosb\n\t"              /* write */\
                           "mov %%ah, %%al\n\t"     /* 'pop' */\
                           "and $0xf, %%al\n\t"     /* (0)+x */\
                           "cmp $0xa, %%al\n\t"     /* (0)+x */\
                           "jb arab%=\n\t"            /* (0)+x */\
                           "add $39, %%al\n\t"      /* (0)+x */\
                           "arab%=:\tadd $48, %%al\n\t"/* (0)+x */\
                           "stosb\n\t"              /* write */\
                   "loopfootctrl4cc%=:\n\t"   \
                   "dec %%cx\n\t"         /* loop foot (all ctrls) */\ 
                   "jne 1b\n\t"      \
                   /* -----------*/         \
                   "endall4cc%=:"             \
                 : "=&S" (dummy0), "=&D" (dummy1), "=&a" (dummy2), "=&d" (inserts), "=&c" (dummy3) \
                 : "0" (src), "1" (dst), "3" (0), "4" (len) \
                 : "memory", "cc"); \
\
    needs = inserts; \
    \
}

#endif

