// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

// for gettimeofday()
#include <sys/time.h>

bool sqlrconnection_svr::logInUpdateStats(bool printerrors) {
	if (logIn(printerrors)) {
		statistics->open_svr_connections++;
		statistics->opened_svr_connections++;
		return true;
	}
	return false;
}

void sqlrconnection_svr::logOutUpdateStats() {
	logOut();
	statistics->open_svr_connections--;
	if (statistics->open_svr_connections<0) {
		statistics->open_svr_connections=0;
	}
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
	if (statistics->open_svr_cursors<0) {
		statistics->open_svr_cursors=0;
	}
}

bool sqlrconnection_svr::executeQueryUpdateStats(sqlrcursor_svr *curs,
							const char *query,
							uint32_t length,
							bool execute) {
	statistics->total_queries++;

	timeval		starttv;
	struct timezone	starttz;
	timeval		endtv;
	struct timezone	endtz;

	if (cfgfl->getTimeQueriesSeconds()>-1 &&
		cfgfl->getTimeQueriesMicroSeconds()>-1) {
		gettimeofday(&starttv,&starttz);
	}

	bool	result=curs->executeQuery(query,length,execute);

printf("threshold=%lld.%lld\n",cfgfl->getTimeQueriesSeconds(),
				cfgfl->getTimeQueriesMicroSeconds());
	if (cfgfl->getTimeQueriesSeconds()>-1 &&
		cfgfl->getTimeQueriesMicroSeconds()>-1) {

		gettimeofday(&endtv,&endtz);

		curs->querysec=endtv.tv_sec-starttv.tv_sec;
		curs->queryusec=endtv.tv_usec-starttv.tv_usec;

		if (curs->querysec>=
				(uint64_t)cfgfl->getTimeQueriesSeconds() &&
			curs->queryusec>=
				(uint64_t)cfgfl->getTimeQueriesMicroSeconds()) {
			stringbuffer	logentry;
			logentry.append("query:\n")->append(query);
			logentry.append("\n");
			logentry.append("time: ");
			logentry.append((uint64_t)curs->querysec);
			logentry.append(".");
			char	*usec=charstring::parseNumber(
						(uint64_t)curs->queryusec,6);
			logentry.append(usec);
			delete[] usec;
			logentry.append("\n");
			querylog.write(logentry.getString(),
					logentry.getStringLength());
		}
	}

	if (!result) {
		statistics->total_errors++;
		return false;
	}
	return true;
}
