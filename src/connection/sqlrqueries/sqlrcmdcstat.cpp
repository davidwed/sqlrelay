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
