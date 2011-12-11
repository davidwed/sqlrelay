// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqltranslatordebug.h>

bool sqlparser::parseUpdate(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	*newptr=ptr;

	// look for a update clause
	if (!updateClause(*newptr,newptr)) {
		debugPrintf("no update clause\n");
		return false;
	}

	// create the node
	xmldomnode	*updatenode=newNode(currentnode,_update);

	// parse the update clauses
	for (;;) {

		// look for known options
		if (parseWhere(updatenode,*newptr,newptr)) {
			continue;
		}

		// If we didn't encounter one of the known options
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (parseVerbatim(updatenode,*newptr,newptr)) {

			// if we find a comma, append that too
			if (comma(*newptr,newptr)) {
				newNode(updatenode,_verbatim,",");
			}

			space(*newptr,newptr);
		} else {
			break;
		}
	}

	return true;
}

bool sqlparser::updateClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"update ");
}

const char *sqlparser::_update="update";
