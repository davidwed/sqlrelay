// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::dropTempTables(sqlrcursor *cursor, stringlist *tablelist) {

	// run through the temp table list, dropping tables
	for (stringlistnode *sln=tablelist->getNodeByIndex(0);
			sln; sln=(stringlistnode *)sln->getNext()) {
		dropTempTable(cursor,sln->getData());
		delete[] sln->getData();
	}
	tablelist->clear();
}

void sqlrconnection::dropTempTable(sqlrcursor *cursor, const char *tablename) {
	stringbuffer	dropquery;
	dropquery.append("drop table ")->append(tablename);
	if (cursor->prepareQuery(dropquery.getString(),
					dropquery.getStringLength())) {
		cursor->executeQuery(dropquery.getString(),
					dropquery.getStringLength(),true);
	}
	cursor->cleanUpData(true,true);
}

void sqlrconnection::truncateTempTables(sqlrcursor *cursor,
						stringlist *tablelist) {

	// run through the temp table list, truncateing tables
	for (stringlistnode *sln=tablelist->getNodeByIndex(0);
			sln; sln=(stringlistnode *)sln->getNext()) {
		truncateTempTable(cursor,sln->getData());
		delete[] sln->getData();
	}
	tablelist->clear();
}

void sqlrconnection::truncateTempTable(sqlrcursor *cursor,
						const char *tablename) {
	stringbuffer	truncatequery;
	truncatequery.append("delete from ")->append(tablename);
	if (cursor->prepareQuery(truncatequery.getString(),
					truncatequery.getStringLength())) {
		cursor->executeQuery(truncatequery.getString(),
					truncatequery.getStringLength(),true);
	}
	cursor->cleanUpData(true,true);
}

void sqlrconnection::addSessionTempTableForDrop(const char *table) {
	sessiontemptablesfordrop.append(strdup(table));
}

void sqlrconnection::addTransactionTempTableForDrop(const char *table) {
	transtemptablesfordrop.append(strdup(table));
}

void sqlrconnection::addSessionTempTableForTrunc(const char *table) {
	sessiontemptablesfortrunc.append(strdup(table));
}

void sqlrconnection::addTransactionTempTableForTrunc(const char *table) {
	transtemptablesfortrunc.append(strdup(table));
}
