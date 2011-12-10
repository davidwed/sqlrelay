// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqlelement.h>
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
	xmldomnode	*insertnode=
			newNode(currentnode,sqlelement::_insert);

	// FIXME: implement this for real
	parseRemainderVerbatim(insertnode,*newptr,newptr);
	return true;
}
