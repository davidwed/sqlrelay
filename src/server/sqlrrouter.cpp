// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrrouterprivate {
	friend class sqlrrouter;
	private:
		sqlrrouters	*_rs;
		xmldomnode	*_parameters;
};

sqlrrouter::sqlrrouter(sqlrservercontroller *cont,
				sqlrrouters *rs,
				xmldomnode *parameters) {
	pvt=new sqlrrouterprivate;
	pvt->_rs=rs;
	pvt->_parameters=parameters;
}

sqlrrouter::~sqlrrouter() {
	delete pvt;
}

const char *sqlrrouter::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur) {
	return NULL;
}

sqlrrouters *sqlrrouter::getRouters() {
	return pvt->_rs;
}

xmldomnode *sqlrrouter::getParameters() {
	return pvt->_parameters;
}
