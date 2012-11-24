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

bool sqlrcmdcstat::match(const char *querystring,
				uint32_t querylength) {
	debugFunction();
	return !charstring::compareIgnoringCase(querystring,"sqlrcmd cstat");
}

sqlrquerycursor *sqlrcmdcstat::getCursor(sqlrconnection_svr *sqlrcon) {
	return new sqlrcmdcstatcursor(sqlrcon,parameters);
}

sqlrcmdcstatcursor::sqlrcmdcstatcursor(
		sqlrconnection_svr *sqlrcon,xmldomnode *parameters) :
					sqlrquerycursor(sqlrcon,parameters) {
	currentrow=0;
}

bool sqlrcmdcstatcursor::executeQuery(const char *query, uint32_t length) {
	currentrow=0;
	return true;
}

bool sqlrcmdcstatcursor::knowsRowCount() {
	return true;
}

uint64_t sqlrcmdcstatcursor::rowCount() {
	return 4;
}

uint32_t sqlrcmdcstatcursor::colCount() {
	return 8;
}

static const char * const colnames[]={
	"INDEX",
	"MINE",
	"PROCESSID",
	"CONNECT",
	"STATE",
	"STATE_TIME",
	"CLIENT_ADDR",
	"SQL_TEXT",
	NULL
};

const char * const * sqlrcmdcstatcursor::columnNames() {
	return colnames;
}

const char *sqlrcmdcstatcursor::getColumnName(uint32_t col) {
	return (col<8)?colnames[col]:NULL;
}

bool sqlrcmdcstatcursor::noRowsToReturn() {
	return false;
}

bool sqlrcmdcstatcursor::fetchRow() {
	if (currentrow<4) {
		currentrow++;
		return true;
	}
	return false;
}

void sqlrcmdcstatcursor::getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null) {
	*blob=false;

	if (col>7) {
		*null=true;
		*field="";
		*fieldlength=0;
	}

	*field="test";
	*fieldlength=4;
	*null=false;
}
