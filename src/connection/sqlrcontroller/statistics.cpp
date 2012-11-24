// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/rawbuffer.h>

// for gettimeofday()
#include <sys/time.h>

void sqlrcontroller_svr::initConnStats() {
	if (handoffindex<STATMAXCONNECTIONS) {
		connstats=&shm->connstats[handoffindex];
		clearConnStats();
		connstats->processid=process::getProcessId();
		setState(INIT);
		connstats->index=handoffindex;
		connstats->logged_in_tv.tv_sec=loggedinsec;
		connstats->logged_in_tv.tv_usec=loggedinusec;
	}
}

void sqlrcontroller_svr::clearConnStats() {
	if (!connstats) {
		return;
	}
	rawbuffer::zero(connstats,sizeof(struct sqlrconnstatistics));
}

void sqlrcontroller_svr::setState(enum sqlrconnectionstate state) {
	if (!connstats) {
		return;
	}
	connstats->state=state;
	gettimeofday(&connstats->state_start_tv,NULL);
}

void sqlrcontroller_svr::setCurrentQuery(sqlrcursor_svr *cursor) {
	if (!connstats) {
		return;
	}
	uint32_t	len=cursor->querylength;
	if (len>STATSQLTEXTLEN) {
		len=STATSQLTEXTLEN;
	}
	charstring::copy(connstats->sqltext,cursor->querybuffer,len);
	connstats->sqltext[len]='\0';
}

void sqlrcontroller_svr::setClientInfo() {
	if (!connstats) {
		return;
	}
	uint64_t	len=clientinfolen;
	if (len>STATCLIENTINFOLEN) {
		len=STATCLIENTINFOLEN;
	}
	charstring::copy(connstats->clientinfo,clientinfo,len);
	connstats->clientinfo[len]='\0';
}

void sqlrcontroller_svr::setClientAddr() {
	if (!connstats) {
		return;
	}
	if (clientsock) {
		char	*clientaddrbuf=clientsock->getPeerAddress();
		if (clientaddrbuf) {
			charstring::copy(connstats->clientaddr,clientaddrbuf);
			delete[] clientaddrbuf;
		} else {
			charstring::copy(connstats->clientaddr,"UNIX");
		}
	} else {
		charstring::copy(connstats->clientaddr,"internal");
	}
}

void sqlrcontroller_svr::incrementOpenServerConnections() {
	semset->waitWithUndo(9);
	stats->open_svr_connections++;
	stats->opened_svr_connections++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::decrementOpenServerConnections() {
	semset->waitWithUndo(9);
	stats->open_svr_connections--;
	if (stats->open_svr_connections<0) {
		stats->open_svr_connections=0;
	}
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementOpenClientConnections() {
	semset->waitWithUndo(9);
	stats->open_cli_connections++;
	stats->opened_cli_connections++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::decrementOpenClientConnections() {
	semset->waitWithUndo(9);
	stats->open_cli_connections--;
	if (stats->open_cli_connections<0) {
		stats->open_cli_connections=0;
	}
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementOpenServerCursors() {
	semset->waitWithUndo(9);
	stats->open_svr_cursors++;
	stats->opened_svr_cursors++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::decrementOpenServerCursors() {
	semset->waitWithUndo(9);
	stats->open_svr_cursors--;
	if (stats->open_svr_cursors<0) {
		stats->open_svr_cursors=0;
	}
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementTimesNewCursorUsed() {
	semset->waitWithUndo(9);
	stats->times_new_cursor_used++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementTimesCursorReused() {
	semset->waitWithUndo(9);
	stats->times_cursor_reused++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementTotalQueries() {
	semset->waitWithUndo(9);
	stats->total_queries++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementTotalErrors() {
	semset->waitWithUndo(9);
	stats->total_errors++;
	semset->signalWithUndo(9);
}
