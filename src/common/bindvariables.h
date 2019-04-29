// Copyrifht (c) 2000-2016  David Muse
// See the file COPYING for more information
#ifndef COUNT_BIND_VARIABLES_H
#define COUNT_BIND_VARIABLES_H
#include <rudiments/character.h>

enum queryparsestate_t {
	IN_QUERY=0,
	IN_QUOTES,
	BEFORE_BIND,
	IN_BIND
};

#ifdef NEED_BEFORE_BIND_VARIABLE
static bool beforeBindVariable(const char *c) {
	return character::inSet(*c," \t\n\r=<>,(+-*/%|&!~^");
}
#endif

#ifdef NEED_IS_BIND_DELIMITER
static bool isBindDelimiter(const char *c,
				bool questionmark,
				bool colon,
				bool atsign,
				bool dollarsign) {
	return (questionmark && *c=='?') ||
		(colon && *c==':' && *(c+1)!='=') ||
		(atsign && *c=='@' && *(c+1)!='@') ||
		(dollarsign && *c=='$');
}
#endif

#ifdef NEED_AFTER_BIND_VARIABLE
static bool afterBindVariable(const char *c) {
	return (character::inSet(*c," \t\n\r,);=") || (*c==':' && *(c+1)=='='));
}
#endif

#ifdef NEED_COUNT_BIND_VARIABLES
static uint16_t countBindVariables(const char *query,
					bool questionmark,
					bool colon,
					bool atsign,
					bool dollarsign) {

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

		if (!inquotes && beforeBindVariable(prevptr)) {
			if (questionmark && isBindDelimiter(
						ptr,true,false,false,false)) {
				questionmarkcount++;
			} else if (colon && isBindDelimiter(
						ptr,false,true,false,false)) {
				coloncount++;
			} else if (atsign && isBindDelimiter(
						ptr,false,false,true,false)) {
				atsigncount++;
			} else if (dollarsign && isBindDelimiter(
						ptr,false,false,false,true)) {
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
