// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqltranslatordebug.h>

bool sqlparser::parseInsert(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	*newptr=ptr;

	// look for a insert clause
	if (!insertClause(*newptr,newptr)) {
		debugPrintf("no insert clause\n");
		return false;
	}

	// create the node
	xmldomnode	*insertnode=newNode(currentnode,_insert);

	// parse the insert clauses
	for (;;) {

		// look for known options
		if (parseInsertInto(insertnode,*newptr,newptr)) {
			continue;
		}

		// If we didn't encounter one of the known options
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (parseVerbatim(insertnode,*newptr,newptr)) {
			space(*newptr,newptr);
		} else {
			break;
		}
	}

	return true;
}

bool sqlparser::insertClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"insert ");
}

const char *sqlparser::_insert="insert";

bool sqlparser::parseInsertInto(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!insertIntoClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_insert_into);
	return true;
}

bool sqlparser::insertIntoClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"into ");
}

const char *sqlparser::_insert_into="insert_into";
