// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::logInUpdateStats() {
	if (logIn()) {
		statistics->open_svr_connections++;
		statistics->opened_svr_connections++;
		return true;
	}
	return false;
}

void sqlrconnection_svr::logOutUpdateStats() {
	logOut();
	statistics->open_svr_connections--;
}

sqlrcursor_svr *sqlrconnection_svr::initCursorUpdateStats() {
	sqlrcursor_svr	*cur=initCursor();
	if (cur) {
		statistics->open_svr_cursors++;
		statistics->opened_svr_cursors++;
	}
	return cur;
}

void sqlrconnection_svr::deleteCursorUpdateStats(sqlrcursor_svr *curs) {
	deleteCursor(curs);
	statistics->open_svr_cursors--;
}

bool sqlrconnection_svr::executeQueryUpdateStats(sqlrcursor_svr *curs,
							const char *query,
							uint32_t length,
							bool execute) {
	statistics->total_queries++;
	if (!curs->executeQuery(query,length,execute)) {
		statistics->total_errors++;
		return false;
	}
	return true;
}
