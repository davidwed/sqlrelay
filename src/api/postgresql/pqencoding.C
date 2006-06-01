/*
Portions Copyright (c) 1996-2002, The PostgreSQL Global Development Group
 
Portions Copyright (c) 1994, The Regents of the University of California

Portions Copyright (c) 2003-2004, David Muse

Permission to use, copy, modify, and distribute this software and its documentation for any purpose, without fee, and without a written agreement is hereby granted, provided that the above copyright notice and this paragraph and the following two paragraphs appear in all copies.
 
IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <rudiments/environment.h>
#include <rudiments/charstring.h>
#include <pqdefinitions.h>

// FIXME: support encodings other than UTF8

extern "C" {

int translateEncoding(const char *encoding) {
	if (encoding) {
		if (!charstring::compare(encoding,"UTF8")) {
			return PG_UTF8;
		}
	}
	return -1;
}

int PQmblen(const unsigned char *s, int encoding) {
	// determine length of multibyte encoded char at *s
	return 1;
}

int PQenv2encoding(void) {
	return translateEncoding(environment::getValue("PGCLIENTENCODING"));
}

}
