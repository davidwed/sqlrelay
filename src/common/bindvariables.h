// Copyrifht (c) 2000-2016  David Muse
// See the file COPYING for more information
#ifndef COUNT_BIND_VARIABLES_H
#define COUNT_BIND_VARIABLES_H
#include <rudiments/character.h>

#ifdef NEED_BEFORE_BIND_VARIABLE
static bool beforeBindVariable(const char *c) {
	return character::inSet(*c," \t\n\r=<>,(+-*/%|&!~^");
}
#endif

#ifdef NEED_AFTER_BIND_VARIABLE
static bool afterBindVariable(const char *c) {
	return (character::inSet(*c," \t\n\r,);=") || (*c==':' && *(c+1)=='='));
}
#endif

#ifdef NEED_COUNT_BIND_VARIABLES
static uint16_t countBindVariables(const char *query) {

	if (!query) {
		return 0;
	}

	const char	*prevptr="\0";
	bool		inquotes=false;

	uint16_t	questionmarkcount=0;
	uint16_t	coloncount=0;
	uint16_t	atsigncount=0;
	uint16_t	dollarsigncount=0;

	for (const char *ptr=query; *ptr; ptr++) {

		// are we inside of quotes?
		if (*ptr=='\'' && (*prevptr!='\\' && *prevptr!='\'')) {
			inquotes=!inquotes;
		}

		// If we're not inside of a quoted string and the previous
		// character was something that might come before a bind
		// variable and we run into a ?, : (for oracle-style binds),
		// @ (for sap/sybase-style binds) or $ (for postgresql-style
		// binds) then we must have found a bind variable.
		//
		// Count ?, :, @, $ separately.
		//
		// (make sure to catch :'s but not :='s)
		// (make sure to catch @'s but not @@'s)
		if (!inquotes && beforeBindVariable(prevptr)) {
			if (*ptr=='?') {
				questionmarkcount++;
			} else if (*ptr==':' && *(ptr+1)!='=') {
				coloncount++;
			} else if (*ptr=='@' && *(ptr+1)!='@') {
				atsigncount++;
			} else if (*ptr=='$') {
				dollarsigncount++;
			}
		}

		prevptr=ptr;
	}

	// if we got $'s or ?'s, ignore the :'s or @'s
	if (dollarsigncount) {
		return dollarsigncount;
	}
	if (questionmarkcount) {
		return questionmarkcount;
	}
	if (coloncount) {
		return coloncount;
	}
	if (atsigncount) {
		return atsigncount;
	}
	return 0;
}
#endif

#endif
