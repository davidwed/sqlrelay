// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrconnection.h>

#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif

#include <unistd.h>

bool sqlrcursor::queryIsNotSelect() {

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

bool sqlrcursor::queryIsCommitOrRollback() {

	// scan the query, bypassing whitespace and comments.
	char	*ptr=skipWhitespaceAndComments(querybuffer);

	// if the query is a commit or rollback, return true
	// otherwise return false
	return (!strncasecmp(ptr,"commit",6) ||
			!strncasecmp(ptr,"rollback",8));
}

char *sqlrcursor::skipWhitespaceAndComments(const char *querybuffer) {
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

void sqlrcursor::checkForTempTable(const char *query, unsigned long length) {

printf("%d: checkForTempTable()\n",getpid());

	char	*ptr=(char *)query;
	char	*endptr=(char *)query+length;

	// skip any leading comments
	if (!skipWhitespace(&ptr,endptr) || !skipComment(&ptr,endptr) ||
		!skipWhitespace(&ptr,endptr)) {
		return;
	}

	// see if the query matches the pattern for a temporary query that
	// creates a temporary table
	if (createtemplower.match(ptr)) {
printf("lower match\n");
		ptr=createtemplower.getSubstringEnd(0);
	} else if (createtempupper.match(ptr)) {
printf("upper match\n");
		ptr=createtempupper.getSubstringEnd(0);
	} else {
printf("no match\n");
		return;
	}

	// get the table name
	stringbuffer	tablename;
	while (*ptr!=' ' && *ptr!='\n' && *ptr!='	' && ptr<endptr) {
		tablename.append(*ptr);
		ptr++;
	}

	// append to list of temp tables
	conn->addSessionTempTableForDrop(tablename.getString());
}

bool sqlrcursor::skipComment(char **ptr, const char *endptr) {
	while (*ptr<endptr && !strncmp(*ptr,"--",2)) {
		while (**ptr && **ptr!='\n') {
			(*ptr)++;
		}
	}
	return *ptr!=endptr;
}

bool sqlrcursor::skipWhitespace(char **ptr, const char *endptr) {
	while ((**ptr==' ' || **ptr=='\n' || **ptr=='	') && *ptr<endptr) {
		(*ptr)++;
	}
	return *ptr!=endptr;
}

bool sqlrcursor::advance(char **ptr, const char *endptr,
						unsigned short steps) {
	for (unsigned short i=0; i<steps && *ptr<endptr; i++) {
		(*ptr)++;
	}
	return *ptr!=endptr;
}
