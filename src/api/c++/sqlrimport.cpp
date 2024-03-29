// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrimport.h>

#include <rudiments/file.h>

sqlrimport::sqlrimport() {
	sqlrcon=NULL;
	sqlrcur=NULL;
	dbtype=NULL;
	objectname=NULL;
	ignorecolumns=false;
	commitcount=0;
	lg=NULL;
	coarseloglevel=0;
	fineloglevel=9;
	logindent=0;
	shutdownflag=NULL;
	logerrors=true;
	lowercasecolumnnames=false;
	uppercasecolumnnames=false;
}

sqlrimport::~sqlrimport() {
	delete[] dbtype;
	delete[] objectname;
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

void sqlrimport::setObjectName(const char *objectname) {
	delete[] this->objectname;
	this->objectname=charstring::duplicate(objectname);
}

void sqlrimport::setIgnoreColumns(bool ignorecolumns) {
	this->ignorecolumns=ignorecolumns;
}

void sqlrimport::mapColumnName(const char *from, const char *to) {
	if (!to) {
		columnmap.remove(from);
	} else {
		columnmap.setValue(from,to);
	}
}

void sqlrimport::mixedCaseColumnNames() {
	lowercasecolumnnames=false;
	uppercasecolumnnames=false;
}

void sqlrimport::lowerCaseColumnNames() {
	lowercasecolumnnames=true;
	uppercasecolumnnames=false;
}

void sqlrimport::upperCaseColumnNames() {
	lowercasecolumnnames=false;
	uppercasecolumnnames=true;
}

void sqlrimport::mapFieldValue(const char *from, const char *to) {
	if (!to) {
		fieldmap.remove(from);
	} else {
		fieldmap.setValue(from,to);
	}
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

void sqlrimport::setLogErrors(bool logerrors) {
	this->logerrors=logerrors;
}
