// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexport.h>

sqlrexport::sqlrexport() {
	sqlrcon=NULL;
	sqlrcur=NULL;
	ignorecolumns=false;
	fieldstoignore=NULL;
	fd=NULL;
	exportrow=true;
	currentrow=0;
	currentcol=0;
	currentfield=NULL;
	lg=NULL;
	coarseloglevel=0;
	fineloglevel=0;
	logindent=0;
	shutdownflag=false;
}

sqlrexport::~sqlrexport() {
}

void sqlrexport::setSqlrConnection(sqlrconnection *sqlrcon) {
	this->sqlrcon=sqlrcon;
}

void sqlrexport::setSqlrCursor(sqlrcursor *sqlrcur) {
	this->sqlrcur=sqlrcur;
}

sqlrconnection *sqlrexport::getSqlrConnection() {
	return sqlrcon;
}

sqlrcursor *sqlrexport::getSqlrCursor() {
	return sqlrcur;
}

void sqlrexport::setIgnoreColumns(bool ignorecolumns) {
	this->ignorecolumns=ignorecolumns;
}

bool sqlrexport::getIgnoreColumns() {
	return ignorecolumns;
}

void sqlrexport::setFieldsToIgnore(const char * const *fieldstoignore) {
	this->fieldstoignore=fieldstoignore;
}

const char * const *sqlrexport::getFieldsToIgnore() {
	return fieldstoignore;
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

logger *sqlrexport::getLogger() {
	return lg;
}

uint8_t sqlrexport::getCoarseLogLevel() {
	return coarseloglevel;
}

uint8_t sqlrexport::getFineLogLevel() {
	return fineloglevel;
}

uint32_t sqlrexport::getLogIndent() {
	return logindent;
}

void sqlrexport::setShutdownFlag(bool *shutdownflag) {
	this->shutdownflag=shutdownflag;
}

bool sqlrexport::headerStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::columnStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::columnEnd() {
	// by default, just return success
	return true;
}

bool sqlrexport::headerEnd() {
	// by default, just return success
	return true;
}

bool sqlrexport::rowsStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::rowStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::fieldStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::fieldEnd() {
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

void sqlrexport::setExportRow(bool exportrow) {
	this->exportrow=exportrow;
}

bool sqlrexport::getExportRow() {
	return exportrow;
}

void sqlrexport::setCurrentRow(uint64_t currentrow) {
	this->currentrow=currentrow;
}

uint64_t sqlrexport::getCurrentRow() {
	return currentrow;
}

void sqlrexport::setCurrentColumn(uint32_t currentcol) {
	this->currentcol=currentcol;
}

uint32_t sqlrexport::getCurrentColumn() {
	return currentcol;
}

void sqlrexport::setCurrentField(const char *currentfield) {
	this->currentfield=currentfield;
}

const char *sqlrexport::getCurrentField() {
	return currentfield;
}

void sqlrexport::setNumberColumn(uint64_t index, bool value) {
	numbercolumns[index]=value;
}

bool sqlrexport::getNumberColumn(uint64_t index) {
	return numbercolumns[index];
}

void sqlrexport::clearNumberColumns() {
	numbercolumns.clear();
}

void sqlrexport::setFileDescriptor(filedescriptor *fd) {
	this->fd=fd;
}

filedescriptor *sqlrexport::getFileDescriptor() {
	return fd;
}
