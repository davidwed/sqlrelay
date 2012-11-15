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
				const char *querystring) {
	return false;
}
