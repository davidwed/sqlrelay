// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::sessionStartQueries() {
	// run a configurable set of queries at the start of each session
	for (stringlistnode *node=
		cfgfl->getSessionStartQueries()->getFirstNode();
						node; node=node->getNext()) {
		sessionQuery(node->getData());
	}
}

void sqlrcontroller_svr::sessionEndQueries() {
	// run a configurable set of queries at the end of each session
	for (stringlistnode *node=
		cfgfl->getSessionEndQueries()->getFirstNode();
						node; node=node->getNext()) {
		sessionQuery(node->getData());
	}
}

void sqlrcontroller_svr::sessionQuery(const char *query) {

	// create the select database query
	size_t	querylen=charstring::length(query);

	sqlrcursor_svr	*cur=initCursorInternal();

	// since we're creating a new cursor for this, make sure it
	// can't have an ID that might already exist
	if (cur->openInternal(cursorcount+1) &&
		cur->prepareQuery(query,querylen) &&
		executeQueryInternal(cur,query,querylen)) {
		cur->cleanUpData(true,true);
	}
	cur->close();
	deleteCursorInternal(cur);
}
