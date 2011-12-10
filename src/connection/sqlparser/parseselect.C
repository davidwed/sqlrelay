// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqlelement.h>
#include <sqltranslatordebug.h>

bool sqlparser::parseSelect(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	*newptr=ptr;

	// look for a select clause
	if (!selectClause(*newptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*selectnode=
			newNode(currentnode,sqlelement::_select);

	// parse the select clauses
	for (;;) {

		// look for known options
		if (parseUnique(selectnode,*newptr,newptr) ||
			parseDistinct(selectnode,*newptr,newptr) ||
			parseWhere(selectnode,*newptr,newptr)) {
			continue;
		}

		// If we didn't encounter one of the known options
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (parseVerbatim(selectnode,*newptr,newptr)) {
			space(*newptr,newptr);
		} else {
			break;
		}
	}

	return true;
}

bool sqlparser::parseUnique(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!uniqueClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_unique);
	return true;
}

bool sqlparser::parseDistinct(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!distinctClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_distinct);
	return true;
}

bool sqlparser::parseWhere(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!whereClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_where);
	return true;
}
