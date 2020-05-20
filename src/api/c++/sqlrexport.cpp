// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexport.h>

sqlrexport::sqlrexport() {
	sqlrcon=NULL;
	sqlrcur=NULL;
	ignorecolumns=false;
	fieldstoignore=NULL;
	exportrow=true;
	currentrow=0;
	currentcol=0;
	currentfield=NULL;
	lg=NULL;
	coarseloglevel=0;
	fineloglevel=0;
	logindent=0;
	shutdownflag=NULL;
	numbercolumns=NULL;
}

sqlrexport::~sqlrexport() {
	delete[] numbercolumns;
}

void sqlrexport::setSqlrConnection(sqlrconnection *sqlrcon) {
	this->sqlrcon=sqlrcon;
}

void sqlrexport::setSqlrCursor(sqlrcursor *sqlrcur) {
	this->sqlrcur=sqlrcur;
}

void sqlrexport::setIgnoreColumns(bool ignorecolumns) {
	this->ignorecolumns=ignorecolumns;
}

void sqlrexport::setFieldsToIgnore(const char * const *fieldstoignore) {
	this->fieldstoignore=fieldstoignore;
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

void sqlrexport::setLogIndent(uint32_t logindent) {
	this->logindent=logindent;
}

void sqlrexport::setShutdownFlag(bool *shutdownflag) {
	this->shutdownflag=shutdownflag;
}

bool sqlrexport::rowsStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::rowStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::colStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::colEnd() {
	// by default, just return success
	return true;
}

bool sqlrexport::rowEnd() {
	// by default, just return success
	return true;
}

bool sqlrexport::rowsEnd() {
	// by default, just return success
	return true;
}
