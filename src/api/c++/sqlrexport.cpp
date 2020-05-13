// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexport.h>

#include <rudiments/file.h>

sqlrexport::sqlrexport() {
	sqlrcur=NULL;
	ignorecolumns=false;
	lg=NULL;
	coarseloglevel=0;
	fineloglevel=0;
	logindent=0;
	shutdownflag=NULL;
}

sqlrexport::~sqlrexport() {
	delete[] dbtype;
}

void sqlrexport::setSqlrCursor(sqlrcursor *sqlrcur) {
	this->sqlrcur=sqlrcur;
}

void sqlrexport::setIgnoreColumns(bool ignorecolumns) {
	this->ignorecolumns=ignorecolumns;
}

void sqlrexport::setLogger(logger *lg) {
	this->lg=lg;
}

void sqlrexport::setCoarseLogLevel(uint8_t coarseloglevel) {
	this->coarseloglevel=coarseloglevel;
}

void sqlrexport::setFineLogLevel(uint8_t fineloglevel) {
	this->fineloglevel=fineloglevel;
}

void sqlrexport::setLogIndent(uint32_t indent) {
	this->logindent=logindent;
}

void sqlrexport::setShutdownFlag(bool *shutdownflag) {
	this->shutdownflag=shutdownflag;
}

bool sqlrexport::exportToFile(const char *filename) {
	return exportToFile(filename,NULL);
}
