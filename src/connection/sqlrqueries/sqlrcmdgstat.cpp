// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrqueries/sqlrcmdgstat.h>
#include <rudiments/charstring.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	sqlrquery	*new_sqlrcmdgstat(xmldomnode *parameters) {
		return new sqlrcmdgstat(parameters);
	}
}

sqlrcmdgstat::sqlrcmdgstat(xmldomnode *parameters) : sqlrquery(parameters) {
	debugFunction();
}

bool sqlrcmdgstat::match(const char *querystring,
				uint32_t querylength) {
	debugFunction();
	return !charstring::compareIgnoringCase(querystring,"sqlrcmd gstat");
}

sqlrquerycursor *sqlrcmdgstat::getCursor(sqlrconnection_svr *sqlrcon) {
	return new sqlrcmdgstatcursor(sqlrcon,parameters);
}

sqlrcmdgstatcursor::sqlrcmdgstatcursor(
		sqlrconnection_svr *sqlrcon,xmldomnode *parameters) :
					sqlrquerycursor(sqlrcon,parameters) {
	currentrow=0;
}

bool sqlrcmdgstatcursor::executeQuery(const char *query, uint32_t length) {
	currentrow=0;
	return true;
}

uint32_t sqlrcmdgstatcursor::colCount() {
	return 2;
}

static const char * const colnames[]={
	"KEY",
	"VALUE",
	NULL
};

const char * const * sqlrcmdgstatcursor::columnNames() {
	return colnames;
}

const char *sqlrcmdgstatcursor::getColumnName(uint32_t col) {
	return (col<2)?colnames[col]:NULL;
}

bool sqlrcmdgstatcursor::noRowsToReturn() {
	return false;
}

bool sqlrcmdgstatcursor::fetchRow() {
	if (currentrow<4) {
		currentrow++;
		return true;
	}
	return false;
}

void sqlrcmdgstatcursor::getField(uint32_t col,
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
