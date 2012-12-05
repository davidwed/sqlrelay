// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

char *sqlrcursor::getQueryTree() {

	reexecute=false;
	validatebinds=false;
	resumed=false;
	clearVariables();

	if (!endofresultset) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!sqlrc->openSession()) {
		return false;
	}

	cached=false;
	endofresultset=false;

	// tell the server we want to get a db list
	sqlrc->cs->write((uint16_t)GET_QUERY_TREE);

	// tell the server whether we'll need a cursor or not
	sendCursorStatus();

	sqlrc->flushWriteBuffer();

	// get the size of the tree
	uint64_t	querytreelen;
	if (sqlrc->cs->read(&querytreelen)!=sizeof(uint64_t)) {
		return NULL;
	}

	// get the tree itself
	char	*querytree=new char[querytreelen+1];
	if (sqlrc->cs->read(querytree,querytreelen)!=querytreelen) {
		delete[] querytree;
		return NULL;
	}
	querytree[querytreelen]='\0';

	return querytree;
}
