// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void	sqlrconnection::dropTempTables(stringlist *tablelist) {

	// run through the temp table list, dropping tables
	for (stringlistnode *sln=tablelist->getNodeByIndex(0);
			sln; sln=(stringlistnode *)sln->getNext()) {
		dropTempTable(sln->getData());
		delete[] sln->getData();
	}
	tablelist->clear();
}

void	sqlrconnection::dropTempTable(const char *tablename) {
	stringbuffer	dropquery;
	dropquery.append("drop table ")->append(tablename);
	sqlrcursor	*dropcur=initCursor();
	if (dropcur->openCursor(-1) &&
		dropcur->prepareQuery(dropquery.getString(),
					dropquery.getStringLength()) &&
		dropcur->executeQuery(dropquery.getString(),
					dropquery.getStringLength(),1)) {
		dropcur->cleanUpData(true,true,true);
	}
	dropcur->closeCursor();
	delete dropcur;
}

void	sqlrconnection::truncateTempTables(stringlist *tablelist) {

	// run through the temp table list, truncateing tables
	for (stringlistnode *sln=tablelist->getNodeByIndex(0);
			sln; sln=(stringlistnode *)sln->getNext()) {
		truncateTempTable(sln->getData());
		delete[] sln->getData();
	}
	tablelist->clear();
}

void	sqlrconnection::truncateTempTable(const char *tablename) {
	stringbuffer	truncatequery;
	truncatequery.append("delete from ")->append(tablename);
	sqlrcursor	*truncatecur=initCursor();
	if (truncatecur->openCursor(-1) &&
		truncatecur->prepareQuery(truncatequery.getString(),
					truncatequery.getStringLength()) &&
		truncatecur->executeQuery(truncatequery.getString(),
					truncatequery.getStringLength(),1)) {
		truncatecur->cleanUpData(true,true,true);
	}
	truncatecur->closeCursor();
	delete truncatecur;
}

void	sqlrconnection::addSessionTempTableForDrop(const char *table) {
	sessiontemptablesfordrop.append(strdup(table));
}

void	sqlrconnection::addTransactionTempTableForDrop(const char *table) {
	transtemptablesfordrop.append(strdup(table));
}

void	sqlrconnection::addSessionTempTableForTrunc(const char *table) {
	sessiontemptablesfortrunc.append(strdup(table));
}

void	sqlrconnection::addTransactionTempTableForTrunc(const char *table) {
	transtemptablesfortrunc.append(strdup(table));
}
