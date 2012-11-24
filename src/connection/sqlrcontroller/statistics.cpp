// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/rawbuffer.h>
#include <rudiments/datetime.h>

// for gettimeofday()
#include <sys/time.h>

void sqlrcontroller_svr::initConnStats() {
	if (handoffindex<STATMAXCONNECTIONS) {
		connstats=&shm->connstats[handoffindex];
		clearConnStats();
		connstats->processid=process::getProcessId();
		updateState(INIT);
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

void sqlrcontroller_svr::updateState(enum sqlrconnectionstate_t state) {
	if (!connstats) {
		return;
	}
	connstats->state=state;
	gettimeofday(&connstats->state_start_tv,NULL);
}

void sqlrcontroller_svr::updateClientSessionStartTime() {
	if (!connstats) {
		return;
	}
	gettimeofday(&connstats->clientsession_tv,NULL);
}

void sqlrcontroller_svr::updateCurrentQuery(const char *query,
						uint32_t querylen) {
	if (!connstats) {
		return;
	}
	uint32_t	len=querylen;
	if (len>STATSQLTEXTLEN) {
		len=STATSQLTEXTLEN;
	}
	charstring::copy(connstats->sqltext,query,len);
	connstats->sqltext[len]='\0';
}

void sqlrcontroller_svr::updateClientInfo(const char *info, uint32_t infolen) {
	if (!connstats) {
		return;
	}
	uint64_t	len=infolen;
	if (len>STATCLIENTINFOLEN) {
		len=STATCLIENTINFOLEN;
	}
	charstring::copy(connstats->clientinfo,info,len);
	connstats->clientinfo[len]='\0';
}

void sqlrcontroller_svr::updateClientAddr() {
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
	if (stats->open_svr_connections) {
		stats->open_svr_connections--;
	}
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementOpenClientConnections() {
	semset->waitWithUndo(9);
	stats->open_cli_connections++;
	stats->opened_cli_connections++;
	semset->signalWithUndo(9);
	if (!connstats) {
		return;
	}
	connstats->nconnect++;
}

void sqlrcontroller_svr::decrementOpenClientConnections() {
	semset->waitWithUndo(9);
	if (stats->open_cli_connections) {
		stats->open_cli_connections--;
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
	if (stats->open_svr_cursors) {
		stats->open_svr_cursors--;
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

void sqlrcontroller_svr::incrementQueryCounts(sqlrquerytype_t querytype) {

	semset->waitWithUndo(9);

	// update total queries
	stats->total_queries++;

	// update queries-per-second stats...

	// re-init stats if necessary
	datetime	dt;
	dt.getSystemDateAndTime();
	time_t	now=dt.getEpoch();
	int	index=now%STATQPSKEEP;
	if (stats->timestamp[index]!=now) {
		stats->timestamp[index]=now;
		stats->qps_select[index]=0;
		stats->qps_update[index]=0;
		stats->qps_insert[index]=0;
		stats->qps_delete[index]=0;
		stats->qps_custom[index]=0;
		stats->qps_etc[index]=0;
	}

	// increment per-query-type stats
	switch (querytype) {
		case SQLRQUERYTYPE_SELECT:
			stats->qps_select[index]++;
			break;
		case SQLRQUERYTYPE_INSERT:
			stats->qps_insert[index]++;
			break;
		case SQLRQUERYTYPE_UPDATE:
			stats->qps_update[index]++;
			break;
		case SQLRQUERYTYPE_DELETE:
			stats->qps_delete[index]++;
			break;
		case SQLRQUERYTYPE_CUSTOM:
			stats->qps_custom[index]++;
			break;
		case SQLRQUERYTYPE_ETC:
		default:
			stats->qps_etc[index]++;
			break;
	}

	semset->signalWithUndo(9);

	if (!connstats) {
		return;
	}
	if (querytype==SQLRQUERYTYPE_CUSTOM) {
		connstats->ncustomsql++;
	} else {
		connstats->nsql++;
	}
}

void sqlrcontroller_svr::incrementTotalErrors() {
	semset->waitWithUndo(9);
	stats->total_errors++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementAuthenticateCount() {
	if (!connstats) {
		return;
	}
	connstats->nauthenticate++;
}

void sqlrcontroller_svr::incrementSuspendSessionCount() {
	if (!connstats) {
		return;
	}
	connstats->nsuspend_session++;
}

void sqlrcontroller_svr::incrementEndSessionCount() {
	if (!connstats) {
		return;
	}
	connstats->nend_session++;
}

void sqlrcontroller_svr::incrementPingCount() {
	if (!connstats) {
		return;
	}
	connstats->nping++;
}

void sqlrcontroller_svr::incrementIdentifyCount() {
	if (!connstats) {
		return;
	}
	connstats->nidentify++;
}

void sqlrcontroller_svr::incrementAutocommitCount() {
	if (!connstats) {
		return;
	}
	connstats->nautocommit++;
}

void sqlrcontroller_svr::incrementBeginCount() {
	if (!connstats) {
		return;
	}
	connstats->nbegin++;
}

void sqlrcontroller_svr::incrementCommitCount() {
	if (!connstats) {
		return;
	}
	connstats->ncommit++;
}

void sqlrcontroller_svr::incrementRollbackCount() {
	if (!connstats) {
		return;
	}
	connstats->nrollback++;
}

void sqlrcontroller_svr::incrementDbVersionCount() {
	if (!connstats) {
		return;
	}
	connstats->ndbversion++;
}

void sqlrcontroller_svr::incrementBindFormatCount() {
	if (!connstats) {
		return;
	}
	connstats->nbindformat++;
}

void sqlrcontroller_svr::incrementServerVersionCount() {
	if (!connstats) {
		return;
	}
	connstats->nserverversion++;
}

void sqlrcontroller_svr::incrementSelectDatabaseCount() {
	if (!connstats) {
		return;
	}
	connstats->nselectdatabase++;
}

void sqlrcontroller_svr::incrementGetCurrentDatabaseCount() {
	if (!connstats) {
		return;
	}
	connstats->ngetcurrentdatabase++;
}

void sqlrcontroller_svr::incrementGetLastInsertIdCount() {
	if (!connstats) {
		return;
	}
	connstats->ngetlastinsertid++;
}

void sqlrcontroller_svr::incrementNewQueryCount() {
	if (!connstats) {
		return;
	}
	connstats->nnewquery++;
}

void sqlrcontroller_svr::incrementReexecuteQueryCount() {
	if (!connstats) {
		return;
	}
	connstats->nreexecutequery++;
}

void sqlrcontroller_svr::incrementFetchFromBindCursorCount() {
	if (!connstats) {
		return;
	}
	connstats->nfetchfrombindcursor++;
}

void sqlrcontroller_svr::incrementFetchResultSetCount() {
	if (!connstats) {
		return;
	}
	connstats->nfetchresultset++;
}

void sqlrcontroller_svr::incrementAbortResultSetCount() {
	if (!connstats) {
		return;
	}
	connstats->nabortresultset++;
}

void sqlrcontroller_svr::incrementSuspendResultSetCount() {
	if (!connstats) {
		return;
	}
	connstats->nsuspendresultset++;
}

void sqlrcontroller_svr::incrementResumeResultSetCount() {
	if (!connstats) {
		return;
	}
	connstats->nresumeresultset++;
}

void sqlrcontroller_svr::incrementGetDbListCount() {
	if (!connstats) {
		return;
	}
	connstats->ngetdblist++;
}

void sqlrcontroller_svr::incrementGetTableListCount() {
	if (!connstats) {
		return;
	}
	connstats->ngettablelist++;
}

void sqlrcontroller_svr::incrementGetColumnListCount() {
	if (!connstats) {
		return;
	}
	connstats->ngetcolumnlist++;
}

void sqlrcontroller_svr::incrementReLogInCount() {
	if (!connstats) {
		return;
	}
	connstats->nrelogin++;
}
