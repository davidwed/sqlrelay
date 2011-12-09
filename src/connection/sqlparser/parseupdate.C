// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqlelement.h>
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
	xmldomnode	*updatenode=
			newNode(currentnode,sqlelement::update_query);

	// FIXME: implement this for real
	parseRemainderVerbatim(updatenode,*newptr,newptr);
	return true;
}
