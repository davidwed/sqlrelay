// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void	sqlrconnection::dropTempTables(stringlist *tablelist) {

	// run through the temp table list, dropping tables
	stringlistnode	*sln=tablelist->getNodeByIndex(0);
	while (sln) {
		dropTempTable(sln->getData());
		delete[] sln->getData();
		sln=(stringlistnode *)sln->getNext();
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
		dropcur->cleanUpData();
	}
	dropcur->closeCursor();
	delete dropcur;
}

void	sqlrconnection::truncateTempTables(stringlist *tablelist) {

	// run through the temp table list, truncateing tables
	stringlistnode	*sln=tablelist->getNodeByIndex(0);
	while (sln) {
		truncateTempTable(sln->getData());
		delete[] sln->getData();
		sln=(stringlistnode *)sln->getNext();
	}
	tablelist->clear();
}

void	sqlrconnection::truncateTempTable(const char *tablename) {
	stringbuffer	truncatequery;
	truncatequery.append("delete from table ")->append(tablename);
	sqlrcursor	*truncatecur=initCursor();
	if (truncatecur->openCursor(-1) &&
		truncatecur->prepareQuery(truncatequery.getString(),
					truncatequery.getStringLength()) &&
		truncatecur->executeQuery(truncatequery.getString(),
					truncatequery.getStringLength(),1)) {
		truncatecur->cleanUpData();
	}
	truncatecur->closeCursor();
	delete truncatecur;
}

void	sqlrconnection::addSessionTempTable(const char *table) {
	sessiontemptables.append(strdup(table));
}

void	sqlrconnection::addTransactionTempTable(const char *table) {
	transtemptables.append(strdup(table));
}
