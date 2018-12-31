// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrrouterprivate {
	friend class sqlrrouter;
	private:
		sqlrrouters	*_rs;
		domnode	*_parameters;
};

sqlrrouter::sqlrrouter(sqlrservercontroller *cont,
				sqlrrouters *rs,
				domnode *parameters) {
	pvt=new sqlrrouterprivate;
	pvt->_rs=rs;
	pvt->_parameters=parameters;
}

sqlrrouter::~sqlrrouter() {
	delete pvt;
}

const char *sqlrrouter::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char **err,
					int64_t *errn) {
	return NULL;
}

bool sqlrrouter::routeEntireSession() {
	return false;
}

sqlrrouters *sqlrrouter::getRouters() {
	return pvt->_rs;
}

domnode *sqlrrouter::getParameters() {
	return pvt->_parameters;
}

void sqlrrouter::endTransaction(bool commit) {
}

void sqlrrouter::endSession() {
}
