// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqlelement.h>
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
	xmldomnode	*deletenode=
			newNode(currentnode,sqlelement::delete_query);

	// FIXME: implement this for real
	parseRemainderVerbatim(deletenode,*newptr,newptr);
	return true;
}
