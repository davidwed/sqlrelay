// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::rollbackCommand() {
	dbgfile.debugPrint("connection",1,"rollback");
	clientsock->write(rollback());
	flushWriteBuffer();
	commitorrollback=false;
}

bool sqlrconnection_svr::rollback() {

	dbgfile.debugPrint("connection",1,"rollback...");

	sqlrcursor_svr	*rollbackcur=initCursorUpdateStats();
	char	*rollbackquery="rollback";
	int	rollbackquerylen=8;
	bool	retval=false;
	if (rollbackcur->openCursor(0) &&
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

	return retval;
}
