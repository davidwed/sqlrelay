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
	connstats=&shm->connstats[handoffindex];
	clearConnStats();
	connstats->processid=process::getProcessId();
	updateState(INIT);
	connstats->index=handoffindex;
	connstats->logged_in_tv.tv_sec=loggedinsec;
	connstats->logged_in_tv.tv_usec=loggedinusec;
}

void sqlrcontroller_svr::clearConnStats() {
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
	gettimeofday(&connstats->clientsession_tv,NULL);
}

void sqlrcontroller_svr::updateCurrentQuery(const char *query,
						uint32_t querylen) {
	uint32_t	len=querylen;
	if (len>STATSQLTEXTLEN-1) {
		len=STATSQLTEXTLEN-1;
	}
	charstring::copy(connstats->sqltext,query,len);
	connstats->sqltext[len]='\0';
}

void sqlrcontroller_svr::updateClientInfo(const char *info, uint32_t infolen) {
	uint64_t	len=infolen;
	if (len>STATCLIENTINFOLEN-1) {
		len=STATCLIENTINFOLEN-1;
	}
	charstring::copy(connstats->clientinfo,info,len);
	connstats->clientinfo[len]='\0';
}

void sqlrcontroller_svr::updateClientAddr() {
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

void sqlrcontroller_svr::incrementOpenDatabaseConnections() {
	semset->waitWithUndo(9);
	shm->open_db_connections++;
	shm->opened_db_connections++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::decrementOpenDatabaseConnections() {
	semset->waitWithUndo(9);
	if (shm->open_db_connections) {
		shm->open_db_connections--;
	}
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementOpenClientConnections() {
	semset->waitWithUndo(9);
	shm->open_cli_connections++;
	shm->opened_cli_connections++;
	semset->signalWithUndo(9);
	connstats->nconnect++;
}

void sqlrcontroller_svr::decrementOpenClientConnections() {
	semset->waitWithUndo(9);
	if (shm->open_cli_connections) {
		shm->open_cli_connections--;
	}
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementOpenDatabaseCursors() {
	semset->waitWithUndo(9);
	shm->open_db_cursors++;
	shm->opened_db_cursors++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::decrementOpenDatabaseCursors() {
	semset->waitWithUndo(9);
	if (shm->open_db_cursors) {
		shm->open_db_cursors--;
	}
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementTimesNewCursorUsed() {
	semset->waitWithUndo(9);
	shm->times_new_cursor_used++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementTimesCursorReused() {
	semset->waitWithUndo(9);
	shm->times_cursor_reused++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementQueryCounts(sqlrquerytype_t querytype) {

	semset->waitWithUndo(9);

	// update total queries
	shm->total_queries++;

	// update queries-per-second stats...

	// re-init stats if necessary
	datetime	dt;
	dt.getSystemDateAndTime();
	time_t	now=dt.getEpoch();
	int	index=now%STATQPSKEEP;
	if (shm->timestamp[index]!=now) {
		shm->timestamp[index]=now;
		shm->qps_select[index]=0;
		shm->qps_update[index]=0;
		shm->qps_insert[index]=0;
		shm->qps_delete[index]=0;
		shm->qps_create[index]=0;
		shm->qps_drop[index]=0;
		shm->qps_alter[index]=0;
		shm->qps_custom[index]=0;
		shm->qps_etc[index]=0;
	}

	// increment per-query-type stats
	switch (querytype) {
		case SQLRQUERYTYPE_SELECT:
			shm->qps_select[index]++;
			break;
		case SQLRQUERYTYPE_INSERT:
			shm->qps_insert[index]++;
			break;
		case SQLRQUERYTYPE_UPDATE:
			shm->qps_update[index]++;
			break;
		case SQLRQUERYTYPE_DELETE:
			shm->qps_delete[index]++;
			break;
		case SQLRQUERYTYPE_CREATE:
			shm->qps_create[index]++;
			break;
		case SQLRQUERYTYPE_DROP:
			shm->qps_drop[index]++;
			break;
		case SQLRQUERYTYPE_ALTER:
			shm->qps_alter[index]++;
			break;
		case SQLRQUERYTYPE_CUSTOM:
			shm->qps_custom[index]++;
			break;
		case SQLRQUERYTYPE_ETC:
		default:
			shm->qps_etc[index]++;
			break;
	}

	semset->signalWithUndo(9);

	if (querytype==SQLRQUERYTYPE_CUSTOM) {
		connstats->ncustomsql++;
	} else {
		connstats->nsql++;
	}
}

void sqlrcontroller_svr::incrementTotalErrors() {
	semset->waitWithUndo(9);
	shm->total_errors++;
	semset->signalWithUndo(9);
}

void sqlrcontroller_svr::incrementAuthenticateCount() {
	connstats->nauthenticate++;
}

void sqlrcontroller_svr::incrementSuspendSessionCount() {
	connstats->nsuspend_session++;
}

void sqlrcontroller_svr::incrementEndSessionCount() {
	connstats->nend_session++;
}

void sqlrcontroller_svr::incrementPingCount() {
	connstats->nping++;
}

void sqlrcontroller_svr::incrementIdentifyCount() {
	connstats->nidentify++;
}

void sqlrcontroller_svr::incrementAutocommitCount() {
	connstats->nautocommit++;
}

void sqlrcontroller_svr::incrementBeginCount() {
	connstats->nbegin++;
}

void sqlrcontroller_svr::incrementCommitCount() {
	connstats->ncommit++;
}

void sqlrcontroller_svr::incrementRollbackCount() {
	connstats->nrollback++;
}

void sqlrcontroller_svr::incrementDbVersionCount() {
	connstats->ndbversion++;
}

void sqlrcontroller_svr::incrementBindFormatCount() {
	connstats->nbindformat++;
}

void sqlrcontroller_svr::incrementServerVersionCount() {
	connstats->nserverversion++;
}

void sqlrcontroller_svr::incrementSelectDatabaseCount() {
	connstats->nselectdatabase++;
}

void sqlrcontroller_svr::incrementGetCurrentDatabaseCount() {
	connstats->ngetcurrentdatabase++;
}

void sqlrcontroller_svr::incrementGetLastInsertIdCount() {
	connstats->ngetlastinsertid++;
}

void sqlrcontroller_svr::incrementNewQueryCount() {
	connstats->nnewquery++;
}

void sqlrcontroller_svr::incrementReexecuteQueryCount() {
	connstats->nreexecutequery++;
}

void sqlrcontroller_svr::incrementFetchFromBindCursorCount() {
	connstats->nfetchfrombindcursor++;
}

void sqlrcontroller_svr::incrementFetchResultSetCount() {
	connstats->nfetchresultset++;
}

void sqlrcontroller_svr::incrementAbortResultSetCount() {
	connstats->nabortresultset++;
}

void sqlrcontroller_svr::incrementSuspendResultSetCount() {
	connstats->nsuspendresultset++;
}

void sqlrcontroller_svr::incrementResumeResultSetCount() {
	connstats->nresumeresultset++;
}

void sqlrcontroller_svr::incrementGetDbListCount() {
	connstats->ngetdblist++;
}

void sqlrcontroller_svr::incrementGetTableListCount() {
	connstats->ngettablelist++;
}

void sqlrcontroller_svr::incrementGetColumnListCount() {
	connstats->ngetcolumnlist++;
}

void sqlrcontroller_svr::incrementReLogInCount() {
	connstats->nrelogin++;
}
