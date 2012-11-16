// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrqueries/sqlrcmdcstat.h>
#include <rudiments/charstring.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	sqlrquery	*new_sqlrcmdcstat(xmldomnode *parameters) {
		return new sqlrcmdcstat(parameters);
	}
}

sqlrcmdcstat::sqlrcmdcstat(xmldomnode *parameters) : sqlrquery(parameters) {
}

bool sqlrcmdcstat::init(sqlrconnection_svr *sqlrcon) {
	return true;
}

bool sqlrcmdcstat::match(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				const char *querystring,
				uint32_t querylength) {
printf("sqlrcmdcstat matching...\n");
	return !charstring::compare(querystring,"sqlrcmd cstat");
}

bool sqlrcmdcstat::executeQuery(const char *query, uint32_t length) {
	return false;
}

bool sqlrcmdcstat::errorMessage(char *errorbuffer,
				uint32_t errorbuffersize,
				uint32_t *errorlength,
				int64_t *errorcode,
				bool *liveconnection) {
	return false;
}

bool sqlrcmdcstat::knowsRowCount() {
	return false;
}

uint64_t sqlrcmdcstat::rowCount() {
	return 0;
}

uint64_t sqlrcmdcstat::affectedRows() {
	return 0;
}

uint32_t sqlrcmdcstat::colCount() {
	return 0;
}

const char * const * sqlrcmdcstat::columnNames() {
	return NULL;
}

void sqlrcmdcstat::returnColumnInfo() {
	return;
}

bool sqlrcmdcstat::noRowsToReturn() {
	return false;
}

bool sqlrcmdcstat::skipRow() {
	return false;
}

bool sqlrcmdcstat::fetchRow() {
	return false;
}

bool sqlrcmdcstat::returnRow() {
	return false;
}

bool sqlrcmdcstat::nextRow() {
	return false;
}

void sqlrcmdcstat::getField(uint32_t col,
			const char **field,
			uint64_t *fieldlength,
			bool *blob,
			bool *null) {
	return;
}

bool sqlrcmdcstat::getColumnNameList(stringbuffer *output) {
	return false;
}
