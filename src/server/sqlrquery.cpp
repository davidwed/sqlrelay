// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/xmldomnode.h>

class sqlrqueryprivate {
	friend class sqlrquery;
	private:
		sqlrqueries	*_qs;
		xmldomnode	*_parameters;
};

sqlrquery::sqlrquery(sqlrservercontroller *cont,
				sqlrqueries *qs,
				xmldomnode *parameters) {
	pvt=new sqlrqueryprivate;
	pvt->_qs=qs;
	pvt->_parameters=parameters;
}

sqlrquery::~sqlrquery() {
	delete pvt;
}

bool sqlrquery::match(const char *querystring, uint32_t querylength) {
	return false;
}

sqlrquerycursor *sqlrquery::newCursor(sqlrserverconnection *conn, uint16_t id) {
	return NULL;
}

sqlrqueries *sqlrquery::getQueries() {
	return pvt->_qs;
}

xmldomnode *sqlrquery::getParameters() {
	return pvt->_parameters;
}

class sqlrquerycursorprivate {
	friend class sqlrquerycursor;
	private:
		sqlrquery	*_q;
		xmldomnode	*_parameters;
};

sqlrquerycursor::sqlrquerycursor(sqlrserverconnection *conn,
					sqlrquery *q,
					xmldomnode *parameters,
					uint16_t id) :
						sqlrservercursor(conn,id) {
	pvt=new sqlrquerycursorprivate;
	pvt->_q=q;
	pvt->_parameters=parameters;
}

sqlrquerycursor::~sqlrquerycursor() {
	delete pvt;
}

sqlrquerytype_t	sqlrquerycursor::queryType(const char *query, uint32_t length) {
	return SQLRQUERYTYPE_CUSTOM;
}

bool sqlrquerycursor::isCustomQuery() {
	return true;
}

sqlrquery *sqlrquerycursor::getQuery() {
	return pvt->_q;
}

sqlrqueries *sqlrquerycursor::getQueries() {
	return pvt->_q->getQueries();
}

xmldomnode *sqlrquerycursor::getParameters() {
	return pvt->_parameters;
}
