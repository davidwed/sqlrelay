// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqltranslatordebug.h>

bool sqlparser::parseDelete(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	*newptr=ptr;

	// look for a delete clause
	if (!deleteClause(*newptr,newptr)) {
		debugPrintf("no delete clause\n");
		return false;
	}

	// create the node
	xmldomnode	*deletenode=newNode(currentnode,_delete);

	// parse the delete clauses
	for (;;) {

		// look for known options
		if (parseWhere(deletenode,*newptr,newptr)) {
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

			space(*newptr,newptr);
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
