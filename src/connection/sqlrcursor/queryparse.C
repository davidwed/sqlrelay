// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrconnection.h>

#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif

int	sqlrcursor::queryIsNotSelect() {

	// scan the query, bypassing whitespace and comments.
	char	*ptr=skipWhitespaceAndComments(querybuffer);

	// if the query is a select but not a select into then return false,
	// otherwise return true
	if (!strncasecmp(ptr,"select",6) && 
			strncasecmp(ptr,"select into ",12)) {
		return 0;
	}
	return 1;
}

int	sqlrcursor::queryIsCommitOrRollback() {

	// scan the query, bypassing whitespace and comments.
	char	*ptr=skipWhitespaceAndComments(querybuffer);

	// if the query is a commit or rollback, return true
	// otherwise return false
	if (!strncasecmp(ptr,"commit",6) || !strncasecmp(ptr,"rollback",8)) {
		return 1;
	}
	return 0;
}

char	*sqlrcursor::skipWhitespaceAndComments(const char *querybuffer) {
	// scan the query, bypassing whitespace and comments.
	char	*ptr=(char *)querybuffer;
	while (*ptr && 
		(*ptr==' ' || *ptr=='\n' || *ptr=='	' || *ptr=='-')) {

		// skip any comments
		if (*ptr=='-') {
			while (*ptr && *ptr!='\n') {
				ptr++;
			}
		}
		ptr++;
	}
	return ptr;
}

void	sqlrcursor::checkForTempTable(const char *query, unsigned long length) {

	char	*ptr=(char *)query;
	char	*endptr=(char *)query+length;

	// skip any leading comments
	if (!skipWhitespace(&ptr,endptr) || !skipComment(&ptr,endptr) ||
		!skipWhitespace(&ptr,endptr)) {
		return;
	}

	// look for "create [local|global] temporary"
	if (!strncasecmp(ptr,"create",6)) {
		if (!advance(&ptr,endptr,6) ||
					!skipWhitespace(&ptr,endptr)) {
			return;
		}
		if (!strncasecmp(ptr,"local",5)) {
			if (!advance(&ptr,endptr,5) ||
					!skipWhitespace(&ptr,endptr)) {
				return;
			}
		} else if (!strncasecmp(ptr,"global",6)) {
			if (!advance(&ptr,endptr,6) ||
					!skipWhitespace(&ptr,endptr)) {
				return;
			}
		}
		if (!strncasecmp(ptr,"temporary",9)) {
			if (!advance(&ptr,endptr,9) ||
					!skipWhitespace(&ptr,endptr)) {
				return;
			}
		} else if (!strncasecmp(ptr,"temp",4)) {
			if (!advance(&ptr,endptr,4) ||
					!skipWhitespace(&ptr,endptr)) {
				return;
			}
		} else {
			// not a temp table
			return;
		}
	} else {
		// not a temp table
		return;
	}

	// check for the word table
	if (!skipWhitespace(&ptr,endptr) || strncasecmp(ptr,"table",5)) {
		return;
	}

	// skip any whitespace before the table name
	if (!advance(&ptr,endptr,5) || !skipWhitespace(&ptr,endptr)) {
		return;
	}

	// get the table name
	stringbuffer	tablename;
	while (*ptr!=' ' && *ptr!='\n' && *ptr!='	' && ptr<endptr) {
		tablename.append(*ptr);
		ptr++;
	}

	// append to list of temp tables
	conn->addSessionTempTable(tablename.getString());
}

int	sqlrcursor::skipComment(char **ptr, const char *endptr) {
	while (*ptr<endptr && !strncmp(*ptr,"--",2)) {
		while (**ptr && **ptr!='\n') {
			(*ptr)++;
		}
	}
	return *ptr!=endptr;
}

int	sqlrcursor::skipWhitespace(char **ptr, const char *endptr) {
	while ((**ptr==' ' || **ptr=='\n' || **ptr=='	') && *ptr<endptr) {
		(*ptr)++;
	}
	return *ptr!=endptr;
}

int	sqlrcursor::advance(char **ptr, const char *endptr,
						unsigned short steps) {
	for (unsigned short i=0; i<steps && *ptr<endptr; i++) {
		(*ptr)++;
	}
	return *ptr!=endptr;
}
