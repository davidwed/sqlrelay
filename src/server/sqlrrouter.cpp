// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrrouterprivate {
	friend class sqlrrouter;
	private:
		xmldomnode	*_parameters;
		bool		_debug;
};

sqlrrouter::sqlrrouter(xmldomnode *parameters, bool debug) {
	pvt=new sqlrrouterprivate;
	pvt->_parameters=parameters;
	pvt->_debug=debug;
}

sqlrrouter::~sqlrrouter() {
	delete pvt;
}

const char *sqlrrouter::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur) {
	return NULL;
}

xmldomnode *sqlrrouter::getParameters() {
	return pvt->_parameters;
}

bool sqlrrouter::getDebug() {
	return pvt->_debug;
}
