// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>


void sqlrcontroller_svr::addSessionTempTableForDrop(const char *table) {
	sessiontemptablesfordrop.append(charstring::duplicate(table));
}

void sqlrcontroller_svr::addTransactionTempTableForDrop(const char *table) {
	transtemptablesfordrop.append(charstring::duplicate(table));
}

void sqlrcontroller_svr::addSessionTempTableForTrunc(const char *table) {
	sessiontemptablesfortrunc.append(charstring::duplicate(table));
}

void sqlrcontroller_svr::addTransactionTempTableForTrunc(const char *table) {
	transtemptablesfortrunc.append(charstring::duplicate(table));
}

void sqlrcontroller_svr::dropTempTables(sqlrcursor_svr *cursor,
					stringlist *tablelist) {

	// some databases require us to re-login before dropping temp tables
	if (tablelist==&sessiontemptablesfordrop &&
			tablelist->getLength() &&
			conn->tempTableDropReLogIn()) {
		reLogIn();
	}

	// run through the temp table list, dropping tables
	for (stringlistnode *sln=tablelist->getFirstNode();
			sln; sln=(stringlistnode *)sln->getNext()) {
		dropTempTable(cursor,sln->getData());
		delete[] sln->getData();
	}
	tablelist->clear();
}

void sqlrcontroller_svr::dropTempTable(sqlrcursor_svr *cursor,
					const char *tablename) {
	stringbuffer	dropquery;
	dropquery.append("drop table ");
	dropquery.append(conn->tempTableDropPrefix());
	dropquery.append(tablename);

	// FIXME: I need to refactor all of this so that this just gets
	// run as a matter of course instead of explicitly getting run here
	// FIXME: freetds/sybase override this method but don't do this
	if (sqltr) {
		if (sqlp->parse(dropquery.getString())) {
			sqltr->runBeforeTriggers(conn,cursor,sqlp->getTree());
		}
	}

	if (cursor->prepareQuery(dropquery.getString(),
					dropquery.getStringLength())) {
		executeQueryInternal(cursor,dropquery.getString(),
					dropquery.getStringLength());
	}
	cursor->cleanUpData(true,true);

	// FIXME: I need to refactor all of this so that this just gets
	// run as a matter of course instead of explicitly getting run here
	// FIXME: freetds/sybase override this method but don't do this
	if (sqltr) {
		sqltr->runAfterTriggers(conn,cursor,sqlp->getTree(),true);
	}
}

void sqlrcontroller_svr::truncateTempTables(sqlrcursor_svr *cursor,
						stringlist *tablelist) {

	// run through the temp table list, truncateing tables
	for (stringlistnode *sln=tablelist->getFirstNode();
			sln; sln=(stringlistnode *)sln->getNext()) {
		truncateTempTable(cursor,sln->getData());
		delete[] sln->getData();
	}
	tablelist->clear();
}

void sqlrcontroller_svr::truncateTempTable(sqlrcursor_svr *cursor,
						const char *tablename) {
	stringbuffer	truncatequery;
	truncatequery.append("delete from ")->append(tablename);
	if (cursor->prepareQuery(truncatequery.getString(),
					truncatequery.getStringLength())) {
		executeQueryInternal(cursor,truncatequery.getString(),
					truncatequery.getStringLength());
	}
	cursor->cleanUpData(true,true);
}
