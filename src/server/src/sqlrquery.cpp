// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrquery.h>
#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <rudiments/xmldomnode.h>

sqlrquery::sqlrquery(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrquery::~sqlrquery() {
}

bool sqlrquery::match(const char *querystring, uint32_t querylength) {
	return false;
}

sqlrquerycursor *sqlrquery::newCursor(sqlrconnection_svr *conn, uint16_t id) {
	return NULL;
}

sqlrquerycursor::sqlrquerycursor(sqlrconnection_svr *conn,
					xmldomnode *parameters,
					uint16_t id) :
						sqlrcursor_svr(conn,id) {
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
