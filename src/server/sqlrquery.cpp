// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/xmldomnode.h>

class sqlrqueryprivate {
	friend class sqlrquery;
	private:
		xmldomnode	*_parameters;
		bool		_debug;
};

sqlrquery::sqlrquery(xmldomnode *parameters, bool debug) {
	pvt=new sqlrqueryprivate;
	pvt->_parameters=parameters;
	pvt->_debug=debug;
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

xmldomnode *sqlrquery::getParameters() {
	return pvt->_parameters;
}

bool sqlrquery::getDebug() {
	return pvt->_debug;
}

class sqlrquerycursorprivate {
	friend class sqlrquerycursor;
	private:
		xmldomnode	*_parameters;
};

sqlrquerycursor::sqlrquerycursor(sqlrserverconnection *conn,
					xmldomnode *parameters,
					uint16_t id) :
						sqlrservercursor(conn,id) {
	pvt=new sqlrquerycursorprivate;
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

xmldomnode *sqlrquerycursor::getParameters() {
	return pvt->_parameters;
}
