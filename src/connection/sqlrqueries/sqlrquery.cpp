// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrquery.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

sqlrquery::sqlrquery(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrquery::~sqlrquery() {
}

bool sqlrquery::init(sqlrconnection_svr *sqlrcon) {
	return true;
}

bool sqlrquery::match(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				const char *querystring,
				uint32_t querylength) {
	return false;
}

bool sqlrquery::prepareQuery(const char *query, uint32_t length) {
	return true;
}

bool sqlrquery::supportsNativeBinds() {
	return false;
}

bool sqlrquery::inputBindString(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {
	return true;
}

bool sqlrquery::inputBindInteger(const char *variable, 
					uint16_t variablesize,
					int64_t *value) {
	return true;
}

bool sqlrquery::inputBindDouble(const char *variable, 
					uint16_t variablesize,
					double *value,
					uint32_t precision,
					uint32_t scale) {
	return true;
}

bool sqlrquery::inputBindDate(const char *variable,
					uint16_t variablesize,
					int64_t year,
					int16_t month,
					int16_t day,
					int16_t hour,
					int16_t minute,
					int16_t second,
					int32_t microsecond,
					const char *tz,
					char *buffer,
					uint16_t buffersize,
					int16_t *isnull) {
	// FIXME: arguably this should convert the
	// date to a string and bind that
	return true;
}

bool sqlrquery::inputBindBlob(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {
	return true;
}

bool sqlrquery::inputBindClob(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {
	return true;
}

bool sqlrquery::outputBindString(const char *variable, 
					uint16_t variablesize,
					char *value,
					uint16_t valuesize,
					int16_t *isnull) {
	return true;
}

bool sqlrquery::outputBindInteger(const char *variable, 
					uint16_t variablesize,
					int64_t *value,
					int16_t *isnull) {
	return true;
}

bool sqlrquery::outputBindDouble(const char *variable, 
					uint16_t variablesize,
					double *value,
					uint32_t *precision,
					uint32_t *scale,
					int16_t *isnull) {
	return true;
}

bool sqlrquery::outputBindDate(const char *variable,
					uint16_t variablesize,
					int16_t *year,
					int16_t *month,
					int16_t *day,
					int16_t *hour,
					int16_t *minute,
					int16_t *second,
					int32_t *microsecond,
					const char **tz,
					char *buffer,
					uint16_t buffersize,
					int16_t *isnull) {
	return true;
}

bool sqlrquery::outputBindBlob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	return true;
}

bool sqlrquery::outputBindClob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	return true;
}

bool sqlrquery::outputBindCursor(const char *variable,
					uint16_t variablesize,
					sqlrcursor_svr *cursor) {
	return true;
}

void sqlrquery::returnOutputBindCursor(uint16_t index) {
	return;
}

bool sqlrquery::getLobOutputBindLength(uint16_t index, uint64_t *length) {
	*length=0;
	return true;
}

bool sqlrquery::getLobOutputBindSegment(uint16_t index,
					char *buffer,
					uint64_t buffersize,
					uint64_t offset,
					uint64_t charstoread,
					uint64_t *charsread) {
	if (buffersize) {
		buffer[0]='\0';
	}
	*charsread=0;
	return true;
}

bool sqlrquery::fetchFromBindCursor() {
	return true;
}

bool sqlrquery::queryIsNotSelect() {
	return true;
}

bool sqlrquery::queryIsCommitOrRollback() {
	return false;
}


bool sqlrquery::errorMessage(char *errorbuffer,
				uint32_t errorbuffersize,
				uint32_t *errorlength,
				int64_t *errorcode,
				bool *liveconnection) {
	if (errorbuffersize) {
		errorbuffer[0]='\0';
	}
	*errorlength=0;
	*errorcode=0;
	*liveconnection=true;
	return false;
}

bool sqlrquery::knowsRowCount() {
	return false;
}

uint64_t sqlrquery::rowCount() {
	return 0;
}

uint64_t sqlrquery::affectedRows() {
	return 0;
}

uint32_t sqlrquery::colCount() {
	return 0;
}

const char * const * sqlrquery::columnNames() {
	return NULL;
}

uint16_t sqlrquery::columnTypeFormat() {
	return COLUMN_TYPE_NAMES;
}

void sqlrquery::returnColumnInfo() {
	return;
}

bool sqlrquery::noRowsToReturn() {
	return false;
}

bool sqlrquery::skipRow() {
	return false;
}

bool sqlrquery::fetchRow() {
	return false;
}

bool sqlrquery::returnRow() {
	return false;
}

bool sqlrquery::nextRow() {
	return false;
}

void sqlrquery::getField(uint32_t col,
			const char **field,
			uint64_t *fieldlength,
			bool *blob,
			bool *null) {
	return;
}

void sqlrquery::sendLobField(uint32_t col) {
	return;
}

bool sqlrquery::getLobFieldLength(uint32_t col, uint64_t *length) {
	return false;
}

bool sqlrquery::getLobFieldSegment(uint32_t col,
				char *buffer,
				uint64_t buffersize,
				uint64_t offset,
				uint64_t charstoread,
				uint64_t *charsread) {
	return false;
}

void sqlrquery::cleanUpLobField(uint32_t col) {
	return;
}

void sqlrquery::cleanUpData(bool freeresult, bool freebinds) {
	return;
}

bool sqlrquery::getColumnNameList(stringbuffer *output) {
	return false;
}
