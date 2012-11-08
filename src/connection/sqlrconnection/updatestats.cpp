// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

// for gettimeofday()
#include <sys/time.h>

bool sqlrconnection_svr::logInUpdateStats(bool printerrors) {
	if (loggedin) {
		return true;
	}
	if (logIn(printerrors)) {
		semset->waitWithUndo(9);
		statistics->open_svr_connections++;
		statistics->opened_svr_connections++;
		semset->signalWithUndo(9);
		loggedin=true;
		return true;
	}
	return false;
}

void sqlrconnection_svr::logOutUpdateStats() {
	if (!loggedin) {
		return;
	}
	logOut();
	semset->waitWithUndo(9);
	statistics->open_svr_connections--;
	if (statistics->open_svr_connections<0) {
		statistics->open_svr_connections=0;
	}
	semset->signalWithUndo(9);
	loggedin=false;
}

sqlrcursor_svr *sqlrconnection_svr::initCursorUpdateStats() {
	sqlrcursor_svr	*cur=initCursor();
	if (cur) {
		semset->waitWithUndo(9);
		statistics->open_svr_cursors++;
		statistics->opened_svr_cursors++;
		semset->signalWithUndo(9);
	}
	return cur;
}

void sqlrconnection_svr::deleteCursorUpdateStats(sqlrcursor_svr *curs) {
	deleteCursor(curs);
	semset->waitWithUndo(9);
	statistics->open_svr_cursors--;
	if (statistics->open_svr_cursors<0) {
		statistics->open_svr_cursors=0;
	}
	semset->signalWithUndo(9);
}

bool sqlrconnection_svr::executeQueryUpdateStats(sqlrcursor_svr *curs,
							const char *query,
							uint32_t length,
							bool execute) {

	// update query count
	semset->waitWithUndo(9);
	statistics->total_queries++;
	semset->signalWithUndo(9);

	// variables for query timing
	timeval		starttv;
	struct timezone	starttz;
	timeval		endtv;
	struct timezone	endtz;

	if (cfgfl->getTimeQueriesSeconds()>-1 &&
		cfgfl->getTimeQueriesMicroSeconds()>-1) {
		// get the query start time
		gettimeofday(&starttv,&starttz);
	}

	// execute the query
	bool	result=curs->executeQuery(query,length,execute);

	if (cfgfl->getTimeQueriesSeconds()>-1 &&
		cfgfl->getTimeQueriesMicroSeconds()>-1) {

		// get the query end time
		gettimeofday(&endtv,&endtz);

		// update stats
		curs->stats.query=query;
		curs->stats.result=result;
		curs->stats.sec=endtv.tv_sec-starttv.tv_sec;
		curs->stats.usec=endtv.tv_usec-starttv.tv_usec;

		// write out a log entry
		if (curs->stats.sec>=
				(uint64_t)cfgfl->getTimeQueriesSeconds() &&
			curs->stats.usec>=
				(uint64_t)cfgfl->getTimeQueriesMicroSeconds()) {
			writeQueryLog(curs);
		}
	}

	// update error count
	if (!result) {
		semset->waitWithUndo(9);
		statistics->total_errors++;
		semset->signalWithUndo(9);
		return false;
	}
	return true;
}
