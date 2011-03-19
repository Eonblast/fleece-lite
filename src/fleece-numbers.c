/**-------------------------------------------------------------------------***
*** File        : Fleece - Lua to JSON module                               ***
*** File        : fleece-numbers.c                                          ***
*** Description : custom programmed float and integer to ascii conversion   ***
*** Version     : 0.3.0 / alpha                                             ***
*** Requirement : self-sufficient ANSI C                                    ***
*** Copyright   : (c) 2011 Henning Diedrich, Eonblast Corporation           ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     :    Feb 2011                                               ***
*** Changed     : 08 Mar 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  Fleece is optimized for the fastest Lua to JSON conversion and beats   ***
***  other JSON implementations by around 10 times, native Lua up to 100x.  ***
***  This file holds some of the basis for the speed gains. Incompatibility ***
***  trade offs will be addressed and a sprintf() variant offered in later  ***
***  editions.                                                              ***
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

/*****************************************************************************\
***                                                                         ***
***                            LOOK UP TABLE                                ***
***                                                                         ***
\*****************************************************************************/

/*---------------------------------------------------------------------------*\
 *                              global tables                                *
\*---------------------------------------------------------------------------*/

char *mux0 = 0;
char *mux = 0;

/*---------------------------------------------------------------------------*\
 *                              fill tables                                  *
\*---------------------------------------------------------------------------*/

void initmux() {

	mux0 = malloc(4000);
	memset(mux0, 0, 4000);
	char *m = mux0 -4;
	int i = 0;
	while(i < 1000)	sprintf(m += 4, "%3.3d", i++);

	mux = malloc(4000);
	memset(mux, 0, 4000);
	m = mux -4;
	i = 0;
	while(i < 1000)	sprintf(m += 4, "%d", i++);
}
	
/*****************************************************************************\
***                                                                         ***
***                             INTEGERS                                    ***
***                                                                         ***
\*****************************************************************************/

/*---------------------------------------------------------------------------*\
 *                          integer to ascii                                 *
\*---------------------------------------------------------------------------*/

void itoa(int pvalue, char* pstr, size_t *rlen) {

	static char num[] = "0123456789";
	
	int value = pvalue;
	char *str = pstr;
 
	// take care of sign
	if(pvalue<0) { *str++ = '-'; value = -value; }

	/* -999 - +999 */
	if (value<1000) {
		// printf(" x %d", value);
		// printf("Rest: %d $ ", value);
		if(!mux) initmux();
		char *q = mux + value*4;
		char qq;
		*str++=*q++; if((qq=*q++)) { *str++=qq; if((qq=*q++)) { *str++=qq; } }
		#ifdef FLEECE_ALWAYS_DELIMIT
		*str=0;
		#endif
		*rlen = str - pstr;
		goto ret;
	}

	if (value<1000000) {
		if(!mux) initmux();
		char *q = mux + (value/1000*4);
		char qq;
		*(str++)=*q++; if((qq=*q++)) { *(str++)=qq; if((qq=*q++)) { *(str++)=qq; } }
		// printf("Now: %s $ ", pstr);
		value %= 1000;
		q = mux0 + (4*value);
		*str++=*q++;
		*str++=*q++;
		*str++=*q++;
		#ifdef FLEECE_ALWAYS_DELIMIT
		*str=0;
		#endif
		*rlen = str - pstr;
		goto ret;
	}

	/* #4 any length
	   Source: Stuart Lowe's optimization of the Kernighan & Ritchie sample
       at http://www.strudel.org.uk/itoa/
       DO NOT CHANGE ANYTHING IN THIS FUNCTION WITHOUT SYNCHING ilen() */	

	/* prototype TODO: move */
	void strreverse(char* begin, char* end);

	/* note: sign is taken care of above. if minus, *str IS alread '-' here. */	
	char* wstr=str;
	div_t res;
	
	// Conversion. Number is reversed in the end.	
	do {
		res = div(value,10);
		*wstr++ = num[res.rem];
	} while((value=res.quot));

	#ifdef FLEECE_ALWAYS_DELIMIT
	*wstr='\0';
	#endif
	*rlen = wstr - pstr;
	
	// Reverse string
	strreverse(str,wstr-1);

	#if SELFCHECK >=3
	    *(pstr+*rlen) = 0;
		char test[32];
		sprintf(test, "%d", pvalue);
		if(strcmp(test, pstr)) {
			printf("fleece [3]: ** fast integer K&R conversion error: %d made to '%s', should be '%s'.\n", pvalue, pstr,test);
			exit(180);
		}
	#endif
	
	ret:;
	#if SELFCHECK >=3
	    *(pstr+*rlen) = 0;
		sprintf(test, "%d", pvalue);
		if(strcmp(test, pstr)) {
			printf("fleece [3]: ** fast integer look up conversion error: %d made to '%s', should be '%s'.\n", pvalue, pstr,test);
			exit(194);
		}
	#endif

	return;
}

void strreverse(char* begin, char* end) {
	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}

/*---------------------------------------------------------------------------*\
 *                              integer length                               *
\*---------------------------------------------------------------------------*/
/* Adapted from Stuart Lowe's optimization of the Kernighan & Ritchie sample
   at http://www.strudel.org.uk/itoa/
   DO NOT CHANGE ANYTHING IN THIS FUNCTION WITHOUT SYNCHING itoa() */
// TODO: COMPROMISED?
size_t ilen(int value) {
	
	// printf("int %d -> ", value);

	int count = 0;
	div_t res;
	
	// Take care of sign
	if (value < 0) { count++; value = -value; }

	// log10	
	do {
		res = div(value,10);
		count++;
	} while((value=res.quot));
	
	// printf("len %d\n", count);
	return count;
}

/*****************************************************************************\
***                                                                         ***
***                                 FLOAT                                   ***
***                                                                         ***
\*****************************************************************************/

/*---------------------------------------------------------------------------*\
 *                        float / double to ascii                            *
\*---------------------------------------------------------------------------*/

/* Adapted from Caldera Unix 6 user land ftoa() dump by Clifford Wolf
   http://www.clifford.at/histsrc/uxv6src/iolib/ftoa.c 
   This is an old routine.
   DO NOT CHANGE ANYTHING IN THIS FUNCTION WITHOUT SYNCHING dlen() */

void dtoa (double x, char *pstr, int prec, char format, size_t *rlen)
{

	/* converts a floating point number to an ascii string */
	/* x is stored into str, which should be at least 30 chars long */
	int ie, i, k, ndig, fstyle;
	double y;
	char * dec = 0;
	char * zeros = 0;
	char *str = pstr;
	ndig = ( prec<=0) ? 7 : (prec >= FLEECE_MAX_PREC ? FLEECE_MAX_PREC : prec+1);
	fstyle = (format == 'f' || format == 'F' || format == 'g' || format == 'G');

	/* print in e format unless last arg is 'f' */
	ie = 0;
	/* if x negative, write minus and reverse */
	if ( x < 0) { *str++ = '-'; x = -x; }

	/* put x in range 1 <= x < 10 */
	if (x > 0.0) while (x < 1.0) { x *= 10.0; ie--;	}
		
	while (x >= 10.0) { x /= 10.0; ie++; }

	/* in f format, number of digits is related to size */
	if (fstyle) ndig += ie;

	/* round. x is between 1 and 10 and ndig will be printed to
	   right of decimal point so rounding is ... */
	for (y = i = 1; i < ndig; i++)
		y = y/10.;

	x += y/2.;
	
	/* repair rounding disasters */
	if (x >= 10.0) {x = 1.0; ie++;}
	
	/* now loop.  put out a digit (obtain by multiplying by
	  10, truncating, subtracting) until enough digits out */
	/* if fstyle, and leading zeros, they go out special */
	if (fstyle && ie<0)
  	{
		*str++ = '0'; *str++ = '.';
		if (ndig < 0) ie = ie-ndig; /* limit zeros if underflow */
		for (i = -1; i > ie; i--)
		    *str++ = '0';
	}
	
	for (i=0; i < ndig; i++)
	{
		k = x;
		*str = k + '0';
		if(k) zeros = 0; else if(!zeros) zeros = str;
		str++;

		/* where is the decimal point? */
		if (i == (fstyle ? ie : 0)) {
			dec = str;
		    *str++ = '.';
		}

		x -= (y=k);
		x *= 10.0;
  	}

	/* now, in estyle,  put out exponent if not zero */
	if (!fstyle && ie != 0)
	{
		*str++ = 'E';
		if (ie < 0) {
			ie = -ie;
			*str++ = '-';
		}
	
		for (k=100; k > ie; k /= 10)
			;
		for (; k > 0; k /= 10) {
       		*str++ = ie/k + '0';
			ie = ie%k;
       	}
  	}
	else // !
	{
		/* trim tail 0s */
		if((format == 'g' || format == 'G') && zeros) str = zeros;
		
		/* alternate robust variant */
		// if(dec) while(str > dec && *(str-1) == '0') str--;
	}	
	#ifdef FLEECE_ALWAYS_DELIMIT
	*str = '\0';
	#endif
	*rlen = str - pstr;
	return;
}

/*---------------------------------------------------------------------------*\
 *                         FAST float / double length                        *
\*---------------------------------------------------------------------------*/

/* optimized conversion. Note that array indices usually run into getsize_integer */
// TODO: COMPROMISED?
size_t fdlen(double x, int prec, char format)
{
	static const double f[] = {-1.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
	const char const *g = (const char const *)f;
	
	if(strstr(g, (const char const *)&x) != 0)
	{
		// printf(" (hit %lg) ", x);
		/* disambiguate  digits AND make sure, could be cross boundary */
		if(x == 0.0 || x == 1.0 || x == 2.0 || x == 3.0 || x == 4.0 ||
	       x == 5.0 || x == 6.0 || x == 7.0 || x == 8.0 || x == 9.0)
			return 3;
		if(x == -1.0 || 10.0)
			return 4;
	}

	return dlen(x, prec, format);
}

/*---------------------------------------------------------------------------*\
 *                           float / double length                           *
\*---------------------------------------------------------------------------*/

/* Adapted from Caldera Unix 6 user land ftoa() dump by Clifford Wolf
   http://www.clifford.at/histsrc/uxv6src/iolib/ftoa.c 
   DO NOT CHANGE ANYTHING IN THIS FUNCTION WITHOUT SYNCHING dlen() */
// TODO: COMPROMISED?

size_t dlen (double x, int prec, char format)
{
	
	char str[32];
	size_t rlen;
	dtoa(x, str, prec, format, &rlen);
	
	/* finds the ascii length of a floating point number */
	/* it is not produced */
	int ie, i, k, ndig, fstyle;
	double y;
	size_t count = 0;
	size_t zeros_count = 0;
	ndig = ( prec<=0) ? 7 : (prec >= FLEECE_MAX_PREC ? FLEECE_MAX_PREC : prec+1);
	fstyle = (format == 'f' || format == 'F' || format == 'g' || format == 'G');

	/* print in e format unless last arg is 'f' */
	ie = 0;

	/* if x negative, write minus and reverse */
	if ( x < 0) { count++; x = -x; }

	/* put x in range 1 <= x < 10 */
	if (x > 0.0) while (x < 1.0) { x *= 10.0; ie--;	}
		
	while (x >= 10.0) { x /= 10.0; ie++; }

	/* in f format, number of digits is related to size */
	if (fstyle) ndig += ie;

	/* round. x is between 1 and 10 and ndig will be printed to
	   right of decimal point so rounding is ... */
	for (y = i = 1; i < ndig; i++)
		y = y/10.;

	x += y/2.;
	
	/* repair rounding disasters */
	if (x >= 10.0) {x = 1.0; ie++;}
	
	/* now loop.  put out a digit (obtain by multiplying by
	  10, truncating, subtracting) until enough digits out */
	/* if fstyle, and leading zeros, they go out special */
	if (fstyle && ie<0)
  	{
		count += 2; // '0.'
		if (ndig < 0) ie = ie-ndig; /* limit zeros if underflow */
		for (i = -1; i > ie; i--)
		    count++; // ..0..
	}
	
    count++; // '.'
	for (i=0; i < ndig; i++)
	{
		k = x;
		if(k) zeros_count = 0; else zeros_count++;
		count++; // digit

		x -= k;
		x *= 10.0;
  	}

	/* now, in estyle,  put out exponent if not zero */
	if (!fstyle && ie != 0)
	{
		count++; // 'E'
		if (ie < 0) {
			ie = -ie;
			count++; // '-';
		}
	
		for (k=100; k > ie; k /= 10)
			;
		for (; k > 0; k /= 10) {
       		count++; // digit
			ie = ie%k;
       	}
  	}
	else // !
	{
	
		/* trim tail 0s */
		if((format == 'g' || format == 'G')) count -= zeros_count;
	}
	
	return count;
}

