// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::commitCommand() {
	dbgfile.debugPrint("connection",1,"commit");
	clientsock->write(commit());
	flushWriteBuffer();
}

bool sqlrconnection_svr::commit() {

	dbgfile.debugPrint("connection",1,"commit...");

	sqlrcursor_svr	*commitcur=initCursorUpdateStats();
	const char	*commitquery="commit";
	int		commitquerylen=6;
	bool		retval=false;
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (commitcur->openCursorInternal(cursorcount+1) &&
		commitcur->prepareQuery(commitquery,commitquerylen)) {
		retval=executeQueryUpdateStats(commitcur,commitquery,
						commitquerylen,true);
	}
	commitcur->cleanUpData(true,true);
	commitcur->closeCursor();
	deleteCursorUpdateStats(commitcur);

	char	string[36];
	snprintf(string,36,"commit result: %d",retval);
	dbgfile.debugPrint("connection",2,string);

	if (retval) {
		commitorrollback=false;
	}

	return retval;
}
