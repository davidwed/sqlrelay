// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrqueries/sqlrcmdcstat.h>
#include <rudiments/charstring.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	sqlrquery	*new_sqlrcmdcstat(xmldomnode *parameters) {
		return new sqlrcmdcstat(parameters);
	}
}

sqlrcmdcstat::sqlrcmdcstat(xmldomnode *parameters) : sqlrquery(parameters) {
	debugFunction();
}

bool sqlrcmdcstat::init(sqlrconnection_svr *sqlrcon) {
	debugFunction();
	return true;
}

bool sqlrcmdcstat::match(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				const char *querystring,
				uint32_t querylength) {
	debugFunction();
	return !charstring::compare(querystring,"sqlrcmd cstat");
}

bool sqlrcmdcstat::executeQuery(const char *query, uint32_t length) {
	return true;
}

bool sqlrcmdcstat::knowsRowCount() {
	debugFunction();
	return false;
}

uint64_t sqlrcmdcstat::rowCount() {
	debugFunction();
	return 0;
}

uint64_t sqlrcmdcstat::affectedRows() {
	debugFunction();
	return 0;
}

uint32_t sqlrcmdcstat::colCount() {
	debugFunction();
	return 0;
}

const char * const * sqlrcmdcstat::columnNames() {
	debugFunction();
	return NULL;
}

void sqlrcmdcstat::returnColumnInfo() {
	debugFunction();
	return;
}

bool sqlrcmdcstat::noRowsToReturn() {
	debugFunction();
	return false;
}

bool sqlrcmdcstat::skipRow() {
	debugFunction();
	return false;
}

bool sqlrcmdcstat::fetchRow() {
	debugFunction();
	return false;
}

bool sqlrcmdcstat::returnRow() {
	debugFunction();
	return false;
}

bool sqlrcmdcstat::nextRow() {
	debugFunction();
	return false;
}

void sqlrcmdcstat::getField(uint32_t col,
			const char **field,
			uint64_t *fieldlength,
			bool *blob,
			bool *null) {
	debugFunction();
	return;
}

bool sqlrcmdcstat::getColumnNameList(stringbuffer *output) {
	debugFunction();
	return false;
}
