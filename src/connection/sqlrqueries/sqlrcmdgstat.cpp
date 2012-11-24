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
printf("sqlrcmd gstat!\n");
}
