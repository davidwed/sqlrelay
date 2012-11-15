// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrqueries/sqlrcmd.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	sqlrquery	*new_sqlrcmd(xmldomnode *parameters) {
		return new sqlrcmd(parameters);
	}
}

sqlrcmd::sqlrcmd(xmldomnode *parameters) : sqlrquery(parameters) {
}

bool sqlrcmd::init(sqlrconnection_svr *sqlrcon) {
	return true;
}

bool sqlrcmd::match(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				const char *querystring) {
	return false;
}
