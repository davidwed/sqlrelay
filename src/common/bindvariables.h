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
	return (character::inSet(*c," \t\n\r,);=<>!") ||
				(*c==':' && *(c+1)=='='));
}
#endif

#ifdef NEED_COUNT_BIND_VARIABLES
static uint16_t countBindVariables(const char *query,
					uint32_t querylen,
					bool questionmark,
					bool colon,
					bool atsign,
					bool dollarsign) {

	if (!query) {
		return 0;
	}

	uint16_t	questionmarkcount=0;
	uint16_t	coloncount=0;
	uint16_t	atsigncount=0;
	uint16_t	dollarsigncount=0;

	queryparsestate_t	parsestate=IN_QUERY;

	const char	*ptr=query;
	const char	*endptr=query+querylen;
	char		prev='\0';
	do {

		// if we're in the query...
		if (parsestate==IN_QUERY) {

			// if we find a quote, we're in quotes
			if (*ptr=='\'') {
				parsestate=IN_QUOTES;
			}

			// if we find whitespace or a couple of other things
			// then the next thing could be a bind variable
			if (beforeBindVariable(ptr)) {
				parsestate=BEFORE_BIND;
			}

			// move on
			if (*ptr=='\\' && prev=='\\') {
				prev='\0';
			} else {
				prev=*ptr;
			}
			ptr++;
			continue;
		}

		// ignore anything in quotes
		if (parsestate==IN_QUOTES) {

			// if we find a quote, but not an escaped quote,
			// then we're back in the query
			// (or we're in between one of these: '...''...'
			// which is functionally the same)
			if (*ptr=='\'' && prev!='\\') {
				parsestate=IN_QUERY;
			}

			// move on
			if (*ptr=='\\' && prev=='\\') {
				prev='\0';
			} else {
				prev=*ptr;
			}
			ptr++;
			continue;
		}

		if (parsestate==BEFORE_BIND) {

			// if we find a bind variable...
			if (questionmark && isBindDelimiter(
						ptr,true,false,false,false)) {
				questionmarkcount++;
				parsestate=IN_BIND;
				continue;
			} else if (colon && isBindDelimiter(
						ptr,false,true,false,false)) {
				coloncount++;
				parsestate=IN_BIND;
				continue;
			} else if (atsign && isBindDelimiter(
						ptr,false,false,true,false)) {
				atsigncount++;
				parsestate=IN_BIND;
				continue;
			} else if (dollarsign && isBindDelimiter(
						ptr,false,false,false,true)) {
				dollarsigncount++;
				parsestate=IN_BIND;
				continue;
			}

			// if we didn't find a bind variable then we're just
			// back in the query
			parsestate=IN_QUERY;
			continue;
		}

		// if we're in a bind variable...
		if (parsestate==IN_BIND) {

			// If we find whitespace or a few other things
			// then we're done with the bind variable.
			if (afterBindVariable(ptr)) {

				parsestate=IN_QUERY;

			} else {

				// move on
				if (*ptr=='\\' && prev=='\\') {
					prev='\0';
				} else {
					prev=*ptr;
				}
				ptr++;
			}
			continue;
		}

	} while (ptr<endptr);

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
