// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>

bool sqlparser::parseDelete(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a delete clause
	if (!deleteClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*deletenode=newNode(currentnode,_delete);

	// parse the delete clauses
	for (;;) {

		// look for the from clause
		if (parseDeleteFrom(deletenode,*newptr,newptr)) {
			break;
		}

		// If we didn't encounter one of the known clauses
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (!parseVerbatim(deletenode,*newptr,newptr)) {
			debugPrintf("missing from clause\n");
			error=true;
			return false;
		}
	}

	// table name
	// FIXME: in mysql, multiple tables may be specified
	if (!parseTableName(deletenode,*newptr,newptr)) {
		debugPrintf("missing table name\n");
		error=true;
		return false;
	}

	// parse the remaining clauses
	for (;;) {

		// look for known options
		if (parseUsing(deletenode,*newptr,newptr) ||
			parseWhere(deletenode,*newptr,newptr) ||
			parseOrderBy(deletenode,*newptr,newptr) ||
			parseLimit(deletenode,*newptr,newptr)) {
			continue;
		}

		// If we didn't encounter one of the known options
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (parseVerbatim(deletenode,*newptr,newptr)) {

			// if we find a comma, append that too
			if (comma(*newptr,newptr)) {
				newNode(deletenode,_verbatim,",");
			}

		} else {
			break;
		}
	}

	return true;
}

bool sqlparser::deleteClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"delete ");
}

const char *sqlparser::_delete="delete";

bool sqlparser::parseDeleteFrom(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!deleteFromClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_delete_from);
	return true;
}

bool sqlparser::deleteFromClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"from ");
}

const char *sqlparser::_delete_from="delete_from";

bool sqlparser::parseUsing(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!usingClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_using);
	return true;
}

bool sqlparser::usingClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"using ");
}

const char *sqlparser::_using="using";
