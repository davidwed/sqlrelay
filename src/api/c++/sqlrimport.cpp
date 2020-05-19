// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrimport.h>

#include <rudiments/file.h>

sqlrimport::sqlrimport() {
	sqlrcon=NULL;
	sqlrcur=NULL;
	dbtype=NULL;
	table=NULL;
	ignorecolumns=false;
	commitcount=0;
	lg=NULL;
	coarseloglevel=0;
	fineloglevel=0;
	logindent=0;
	shutdownflag=NULL;
}

sqlrimport::~sqlrimport() {
	delete[] dbtype;
	delete[] table;
}

void sqlrimport::setSqlrConnection(sqlrconnection *sqlrcon) {
	this->sqlrcon=sqlrcon;
}

void sqlrimport::setSqlrCursor(sqlrcursor *sqlrcur) {
	this->sqlrcur=sqlrcur;
}

void sqlrimport::setDbType(const char *dbtype) {
	delete[] this->dbtype;
	this->dbtype=charstring::duplicate(dbtype);
}

void sqlrimport::setTable(const char *table) {
	delete[] this->table;
	this->table=charstring::duplicate(table);
}

void sqlrimport::setIgnoreColumns(bool ignorecolumns) {
	this->ignorecolumns=ignorecolumns;
}

void sqlrimport::setCommitCount(uint64_t commitcount) {
	this->commitcount=commitcount;
}

void sqlrimport::setLogger(logger *lg) {
	this->lg=lg;
}

void sqlrimport::setCoarseLogLevel(uint8_t coarseloglevel) {
	this->coarseloglevel=coarseloglevel;
}

void sqlrimport::setFineLogLevel(uint8_t fineloglevel) {
	this->fineloglevel=fineloglevel;
}

void sqlrimport::setLogIndent(uint32_t logindent) {
	this->logindent=logindent;
}

void sqlrimport::setShutdownFlag(bool *shutdownflag) {
	this->shutdownflag=shutdownflag;
}
