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
