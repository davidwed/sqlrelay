// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqlelement.h>
#include <sqltranslatordebug.h>

bool sqlparser::parseDrop(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	*newptr=ptr;

	// look for a drop clause
	if (!dropClause(*newptr,newptr)) {
		debugPrintf("no drop clause\n");
		return false;
	}

	// create the node
	xmldomnode	*dropnode=
			newNode(currentnode,sqlelement::drop_query);

	// FIXME: implement this for real
	parseRemainderVerbatim(dropnode,*newptr,newptr);
	return true;
}
