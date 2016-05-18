// Copyrifht (c) 2000-2016  David Muse
// See the file COPYING for more information
#ifndef COUNT_BIND_VARIABLES_H
#define COUNT_BIND_VARIABLES_H

static uint16_t countBindVariables(const char *query) {

	if (!query) {
		return 0;
	}

	char	lastchar='\0';
	bool	inquotes=false;

	uint16_t	questionmarkcount=0;
	uint16_t	coloncount=0;
	uint16_t	atsigncount=0;
	uint16_t	dollarsigncount=0;

	for (const char *ptr=query; *ptr; ptr++) {

		if (*ptr=='\'' && lastchar!='\\') {
			if (inquotes) {
				inquotes=false;
			} else {
				inquotes=true;
			}
		}

		// If we're not inside of a quoted string and we run into
		// a ?, : (for oracle-style binds), @ (for sap/sybase-style
		// binds) or $ (for postgresql-style binds) and the previous
		// character was something that might come before a bind
		// variable then we must have found a bind variable.
		// count ?, :, @, $ separately
		if (!inquotes &&
			character::inSet(lastchar," \t\n\r=<>,(+-*/%|&!~^")) {
			if (*ptr=='?') {
				questionmarkcount++;
			} else if (*ptr==':') {
				coloncount++;
			} else if (*ptr=='@') {
				atsigncount++;
			} else if (*ptr=='$') {
				dollarsigncount++;
			}
		}

		lastchar=*ptr;
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
