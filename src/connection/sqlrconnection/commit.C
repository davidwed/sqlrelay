// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::commitCommand() {
	dbgfile.debugPrint("connection",1,"commit");
	clientsock->write(commit());
	flushWriteBuffer();
	commitorrollback=false;
}

bool sqlrconnection_svr::commit() {

	dbgfile.debugPrint("connection",1,"commit...");

	sqlrcursor_svr	*commitcur=initCursorUpdateStats();
	char	*commitquery="commit";
	int	commitquerylen=6;
	bool	retval=false;
	if (commitcur->openCursorInternal(0) &&
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

	return retval;
}
