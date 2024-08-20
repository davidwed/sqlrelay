// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/domnode.h>

class sqlrqueryprivate {
	friend class sqlrquery;
	private:
};

sqlrquery::sqlrquery(sqlrservercontroller *cont,
				domnode *parameters) :
				sqlrservermodule(cont,parameters) {
	pvt=new sqlrqueryprivate;
}

sqlrquery::~sqlrquery() {
	delete pvt;
}

bool sqlrquery::match(const char *querystring, uint32_t querysize) {
	return false;
}

sqlrquerycursor *sqlrquery::newCursor(sqlrserverconnection *conn, uint16_t id) {
	return NULL;
}

class sqlrquerycursorprivate {
	friend class sqlrquerycursor;
	private:
		sqlrquery	*_q;
		domnode		*_parameters;
};

sqlrquerycursor::sqlrquerycursor(sqlrserverconnection *conn,
					sqlrquery *q,
					domnode *parameters,
					uint16_t id) :
					sqlrservercursor(conn,id) {
	pvt=new sqlrquerycursorprivate;
	pvt->_q=q;
	pvt->_parameters=parameters;
}

sqlrquerycursor::~sqlrquerycursor() {
	delete pvt;
}

sqlrquerytype_t	sqlrquerycursor::determineQueryType(const char *query,
							uint32_t size) {
	return SQLRQUERYTYPE_CUSTOM;
}

bool sqlrquerycursor::isCustomQuery() {
	return true;
}

sqlrquery *sqlrquerycursor::getQuery() {
	return pvt->_q;
}

domnode *sqlrquerycursor::getParameters() {
	return pvt->_parameters;
}
