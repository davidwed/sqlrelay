// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::pingCommand() {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"ping");
	#endif
	clientsock->write(ping());
	flushWriteBuffer();
}

bool sqlrconnection_svr::ping() {
	sqlrcursor_svr	*pingcur=initCursorUpdateStats();
	const char	*pingquery=pingQuery();
	int		pingquerylen=charstring::length(pingQuery());
	if (pingcur->openCursor(0) &&
		pingcur->prepareQuery(pingquery,pingquerylen) &&
		executeQueryUpdateStats(pingcur,pingquery,pingquerylen,true)) {
		pingcur->cleanUpData(true,true);
		pingcur->closeCursor();
		deleteCursorUpdateStats(pingcur);
		return true;
	}
	pingcur->closeCursor();
	deleteCursorUpdateStats(pingcur);
	return false;
}

const char *sqlrconnection_svr::pingQuery() {
	return "select 1";
}
