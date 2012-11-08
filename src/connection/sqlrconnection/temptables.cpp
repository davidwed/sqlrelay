// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::dropTempTables(sqlrcursor_svr *cursor,
					stringlist *tablelist) {

	// run through the temp table list, dropping tables
	for (stringlistnode *sln=tablelist->getFirstNode();
			sln; sln=(stringlistnode *)sln->getNext()) {
		dropTempTable(cursor,sln->getData());
		delete[] sln->getData();
	}
	tablelist->clear();
}

void sqlrconnection_svr::dropTempTable(sqlrcursor_svr *cursor,
					const char *tablename) {
	stringbuffer	dropquery;
	dropquery.append("drop table ")->append(tablename);

	// FIXME: I need to refactor all of this so that this just gets
	// run as a matter of course instead of explicitly getting run here
	if (sqltr) {
		if (sqlp->parse(dropquery.getString())) {
			sqltr->runBeforeTriggers(this,cursor,sqlp->getTree());
		}
	}

	if (cursor->prepareQuery(dropquery.getString(),
					dropquery.getStringLength())) {
		executeQueryInternal(cursor,dropquery.getString(),
					dropquery.getStringLength(),true);
	}
	cursor->cleanUpData(true,true);

	// FIXME: I need to refactor all of this so that this just gets
	// run as a matter of course instead of explicitly getting run here
	if (sqltr) {
		sqltr->runAfterTriggers(this,cursor,sqlp->getTree(),true);
	}
}

void sqlrconnection_svr::truncateTempTables(sqlrcursor_svr *cursor,
						stringlist *tablelist) {

	// run through the temp table list, truncateing tables
	for (stringlistnode *sln=tablelist->getFirstNode();
			sln; sln=(stringlistnode *)sln->getNext()) {
		truncateTempTable(cursor,sln->getData());
		delete[] sln->getData();
	}
	tablelist->clear();
}

void sqlrconnection_svr::truncateTempTable(sqlrcursor_svr *cursor,
						const char *tablename) {
	stringbuffer	truncatequery;
	truncatequery.append("delete from ")->append(tablename);
	if (cursor->prepareQuery(truncatequery.getString(),
					truncatequery.getStringLength())) {
		executeQueryInternal(cursor,truncatequery.getString(),
					truncatequery.getStringLength(),true);
	}
	cursor->cleanUpData(true,true);
}

void sqlrconnection_svr::addSessionTempTableForDrop(const char *table) {
	sessiontemptablesfordrop.append(charstring::duplicate(table));
}

void sqlrconnection_svr::addTransactionTempTableForDrop(const char *table) {
	transtemptablesfordrop.append(charstring::duplicate(table));
}

void sqlrconnection_svr::addSessionTempTableForTrunc(const char *table) {
	sessiontemptablesfortrunc.append(charstring::duplicate(table));
}

void sqlrconnection_svr::addTransactionTempTableForTrunc(const char *table) {
	transtemptablesfortrunc.append(charstring::duplicate(table));
}
