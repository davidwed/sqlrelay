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
	// by default, do nothing
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
