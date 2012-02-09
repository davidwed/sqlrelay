// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::rollbackCommand() {
	dbgfile.debugPrint("connection",1,"rollback");
	clientsock->write(rollbackInternal());
	flushWriteBuffer();
}

bool sqlrconnection_svr::rollbackInternal() {

	if (rollback()) {
		endFakeTransactionBlock();
		return true;
	}
	return false;
}

bool sqlrconnection_svr::rollback() {

	dbgfile.debugPrint("connection",1,"rollback...");

	sqlrcursor_svr	*rollbackcur=initCursorUpdateStats();
	const char	*rollbackquery="rollback";
	int		rollbackquerylen=8;
	bool		retval=false;
	// since we're creating a new cursor for this, make sure it can't
	// have an ID that might already exist
	if (rollbackcur->openCursorInternal(cursorcount+1) &&
		rollbackcur->prepareQuery(rollbackquery,rollbackquerylen)) {
		retval=executeQueryUpdateStats(rollbackcur,rollbackquery,
						rollbackquerylen,true);
	}
	rollbackcur->cleanUpData(true,true);
	rollbackcur->closeCursor();
	deleteCursorUpdateStats(rollbackcur);

	char	string[38];
	snprintf(string,38,"rollback result: %d",retval);
	dbgfile.debugPrint("connection",2,string);

	if (retval) {
		commitorrollback=false;
	}

	return retval;
}
