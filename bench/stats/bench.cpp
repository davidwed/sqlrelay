// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include "bench.h"

benchconnection::benchconnection(const char *connectstring,
						const char *dbtype) {
	pstring.parse(connectstring);
	this->dbtype=dbtype;

	createquery=NULL;
	dropquery=NULL;
	insertquery=NULL;
	updatequery=NULL;
	deletequery=NULL;
	selectquery=NULL;
}

benchconnection::~benchconnection() {
	delete[] createquery;
	delete[] dropquery;
	delete[] insertquery;
	delete[] updatequery;
	delete[] deletequery;
	delete[] selectquery;
}

void benchconnection::setRowCount(uint64_t rowcount) {
	this->rowcount=rowcount;
}

void benchconnection::setColumnCount(uint32_t columncount) {
	this->columncount=columncount;
}

void benchconnection::buildQueries() {
	if (!charstring::compare(dbtype,"oracle")) {
		buildOracleQueries();
	}
}

void benchconnection::buildOracleQueries() {

	createquery=charstring::duplicate(
			"create table testtable ("
			"	col1 varchar2(100)"
			")");

	dropquery=charstring::duplicate(
			"drop table testtable");

	insertquery=charstring::duplicate(
			"insert into testtable values ("
			"'abcdefghijk'"
			")");

	selectquery=charstring::duplicate("select 1 from dual");
}

const char *benchconnection::getParam(const char *param) {
	return pstring.getValue(param);
}

const char *benchconnection::getDbType() {
	return dbtype;
}

const char *benchconnection::getCreateQuery() {
	return createquery;
}

const char *benchconnection::getDropQuery() {
	return dropquery;
}

const char *benchconnection::getInsertQuery() {
	return insertquery;
}

const char *benchconnection::getUpdateQuery() {
	return updatequery;
}

const char *benchconnection::getDeleteQuery() {
	return deletequery;
}

const char *benchconnection::getSelectQuery() {
	return selectquery;
}

benchcursor::benchcursor(benchconnection *bcon) {
	this->bcon=bcon;
}

benchcursor::~benchcursor() {
}
