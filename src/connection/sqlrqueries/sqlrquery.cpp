// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrquery.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <rudiments/xmldomnode.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

sqlrquery::sqlrquery(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrquery::~sqlrquery() {
}

bool sqlrquery::match(const char *querystring, uint32_t querylength) {
	return false;
}

sqlrquerycursor *sqlrquery::getCursor(sqlrconnection_svr *conn) {
	return NULL;
}

sqlrquerycursor::sqlrquerycursor(sqlrconnection_svr *conn,
					xmldomnode *parameters) :
						sqlrcursor_svr(conn) {
	this->parameters=parameters;
}

sqlrquerycursor::~sqlrquerycursor() {
}
