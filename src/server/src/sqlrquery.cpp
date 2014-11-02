// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrquery.h>
#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>
#include <rudiments/xmldomnode.h>

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

sqlrquerytype_t	sqlrquerycursor::queryType(const char *query, uint32_t length) {
	return SQLRQUERYTYPE_CUSTOM;
}

bool sqlrquerycursor::isCustomQuery() {
	return true;
}
