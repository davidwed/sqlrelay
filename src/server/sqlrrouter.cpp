// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrrouterprivate {
	friend class sqlrrouter;
	private:
		sqlrrouters	*_rs;
		xmldomnode	*_parameters;
		const char	**_connectionids;
		sqlrconnection	**_connections;
		uint16_t	_connectioncount;
};

sqlrrouter::sqlrrouter(sqlrservercontroller *cont,
				sqlrrouters *rs,
				xmldomnode *parameters,
				const char **connectionids,
				sqlrconnection **connections,
				uint16_t connectioncount) {
	pvt=new sqlrrouterprivate;
	pvt->_rs=rs;
	pvt->_parameters=parameters;
	pvt->_connectionids=connectionids;
	pvt->_connections=connections;
	pvt->_connectioncount=connectioncount;
}

sqlrrouter::~sqlrrouter() {
	delete pvt;
}

const char *sqlrrouter::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur) {
	return NULL;
}

bool sqlrrouter::routeEntireSession() {
	return false;
}

sqlrrouters *sqlrrouter::getRouters() {
	return pvt->_rs;
}

xmldomnode *sqlrrouter::getParameters() {
	return pvt->_parameters;
}

const char  **sqlrrouter::getConnectionIds() {
	return pvt->_connectionids;
}

sqlrconnection  **sqlrrouter::getConnections() {
	return pvt->_connections;
}

uint16_t sqlrrouter::getConnectionCount() {
	return pvt->_connectioncount;
}
