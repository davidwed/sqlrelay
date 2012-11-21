// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrqueries/sqlrcmdgstat.h>
#include <rudiments/charstring.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	sqlrquery	*new_sqlrcmdgstat(xmldomnode *parameters) {
		return new sqlrcmdgstat(parameters);
	}
}

sqlrcmdgstat::sqlrcmdgstat(xmldomnode *parameters) : sqlrquery(parameters) {
}

bool sqlrcmdgstat::init(sqlrconnection_svr *sqlrcon) {
	return true;
}

bool sqlrcmdgstat::match(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				const char *querystring,
				uint32_t querylength) {
	return !charstring::compareIgnoringCase(querystring,"sqlrcmd gstat");
}

bool sqlrcmdgstat::executeQuery(const char *query, uint32_t length) {
	return false;
}

bool sqlrcmdgstat::errorMessage(char *errorbuffer,
				uint32_t errorbuffersize,
				uint32_t *errorlength,
				int64_t *errorcode,
				bool *liveconnection) {
	return false;
}

bool sqlrcmdgstat::knowsRowCount() {
	return false;
}

uint64_t sqlrcmdgstat::rowCount() {
	return 0;
}

uint64_t sqlrcmdgstat::affectedRows() {
	return 0;
}

uint32_t sqlrcmdgstat::colCount() {
	return 0;
}

const char * const * sqlrcmdgstat::columnNames() {
	return NULL;
}

void sqlrcmdgstat::returnColumnInfo() {
	return;
}

bool sqlrcmdgstat::noRowsToReturn() {
	return false;
}

bool sqlrcmdgstat::skipRow() {
	return false;
}

bool sqlrcmdgstat::fetchRow() {
	return false;
}

bool sqlrcmdgstat::returnRow() {
	return false;
}

bool sqlrcmdgstat::nextRow() {
	return false;
}

void sqlrcmdgstat::getField(uint32_t col,
			const char **field,
			uint64_t *fieldlength,
			bool *blob,
			bool *null) {
	return;
}

bool sqlrcmdgstat::getColumnNameList(stringbuffer *output) {
	return false;
}
