/*
Portions Copyright (c) 1996-2002, The PostgreSQL Global Development Group
 
Portions Copyright (c) 1994, The Regents of the University of California

Portions Copyright (c) 2003-2004, David Muse

Permission to use, copy, modify, and distribute this software and its documentation for any purpose, without fee, and without a written agreement is hereby granted, provided that the above copyright notice and this paragraph and the following two paragraphs appear in all copies.
 
IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <pqdefinitions.h>
#include <rudiments/charstring.h>
#include <rudiments/character.h>
#include <stdlib.h>

extern "C" {

// Haha!  I stole these straight out of the postgresql source
//		(and reformatted them a bit)

/*
PostgreSQL is Copyright © 1996-2002 by the PostgreSQL Global Development Group
and is distributed under the terms of the license of the University of
California below.

Postgres95 is Copyright © 1994-5 by the Regents of the University of California.

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose, without fee, and without a written agreement is
hereby granted, provided that the above copyright notice and this paragraph and
the following two paragraphs appear in all copies.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST
PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS-IS" BASIS, AND
THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE,
SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
*/

size_t PQescapeString(char *to, const char *from, size_t length) {
	debugFunction();

	const char	*source=from;
	char		*target=to;
	unsigned int	remaining=length;

	while (remaining>0) {
		switch (*source) {
			case '\\':
				*target='\\';
				target++;
				*target='\\';
				// target and remaining are updated below.
				break;

			case '\'':
				*target='\'';
				target++;
				*target='\'';
				// target and remaining are updated below.
				break;

			default:
				*target=*source;
				// target and remaining are updated below.
		}
		source++;
		target++;
		remaining--;
	}

	// Write the terminating NUL character.
	*target='\0';

	return target-to;
}

unsigned char *PQescapeBytea(unsigned char *bintext, size_t binlen,
			  				size_t *bytealen) {
	debugFunction();
	unsigned char	*vp;
	unsigned char	*rp;
	unsigned char	*result;
	size_t		i;
	size_t		len;

	// empty string has 1 char ('\0')
	len=1;

	vp=bintext;
	for (i=binlen; i>0; i--, vp++) {
		if (*vp==0 || *vp>=0x80) {
			len+=5;			// '5' is for '\\ooo'
		} else if (*vp=='\'') {
			len+=2;
		} else if (*vp=='\\') {
			len+=4;
		} else {
			len++;
		}
	}

	rp=result=(unsigned char *)malloc(len);
	if (rp==NULL) {
		return NULL;
	}

	vp=bintext;
	*bytealen=len;

	for (i=binlen; i>0; i--, vp++) {
		if (*vp==0 || *vp>=0x80) {
			(void)sprintf((char *)rp,"\\\\%03o",*vp);
			rp+=5;
		} else if (*vp=='\'') {
			rp[0]='\\';
			rp[1]='\'';
			rp+=2;
		} else if (*vp=='\\') {
			rp[0]='\\';
			rp[1]='\\';
			rp[2]='\\';
			rp[3]='\\';
			rp+=4;
		} else {
			*rp++=*vp;
		}
	}
	*rp='\0';

	return result;
}

unsigned char *PQunescapeBytea(unsigned char *strtext, size_t *retbuflen) {
	debugFunction();

	size_t		buflen;
	unsigned char	*buffer, *sp, *bp;
	unsigned int	state=0;

	if (strtext==NULL) {
		return NULL;
	}

	// will shrink, also we discover if strtext isn't NULL terminated
	buflen=charstring::length((char *)strtext);
	buffer=(unsigned char *)malloc(buflen);

	if (buffer==NULL) {
		return NULL;
	}

	for (bp=buffer, sp=strtext; *sp!='\0'; bp++, sp++) {
		switch (state) {
			case 0:
				if (*sp=='\\') {
					state=1;
				}
				*bp=*sp;
				break;
			case 1:
				if (*sp=='\'') {
					// state=5
					// replace \' with 39
					bp--;
					*bp='\'';
					buflen--;
					state=0;
				} else if (*sp=='\\')	{
					// state=6
					// replace \\ with 92
					bp--;
					*bp='\\';
					buflen--;
					state=0;
				} else {
					if (character::isDigit(*sp)) {
						state=2;
					} else {
						state=0;
					}
					*bp=*sp;
				}
				break;
			case 2:
				if (character::isDigit(*sp)) {
					state=3;
				} else {
					state=0;
				}
				*bp=*sp;
				break;
			case 3:
				if (character::isDigit(*sp)) {
					// state=4
					int	v;
					bp-=3;
					sscanf((char *)(sp-2),
						"%03o",&v);
					*bp=v;
					buflen-=3;
					state=0;
				} else {
					*bp=*sp;
					state=0;
				}
				break;
		}
	}
	buffer=(unsigned char *)realloc(buffer, buflen);
	if (buffer==NULL) {
		return NULL;
	}

	*retbuflen=buflen;
	return buffer;
}


}
