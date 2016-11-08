// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

class sqlrauthprivate {
	friend class sqlrauth;
	public:
		xmldomnode	*_parameters;
		sqlrpwdencs	*_sqlrpe;
		bool		_debug;
};

sqlrauth::sqlrauth(xmldomnode *parameters, sqlrpwdencs *sqlrpe, bool debug) {
	pvt=new sqlrauthprivate;
	pvt->_parameters=parameters;
	pvt->_sqlrpe=sqlrpe;
	pvt->_debug=debug;
}

sqlrauth::~sqlrauth() {
	delete pvt;
}

xmldomnode *sqlrauth::getParameters() {
	return pvt->_parameters;
}

sqlrpwdencs *sqlrauth::getPasswordEncryptions() {
	return pvt->_sqlrpe;
}

bool sqlrauth::getDebug() {
	return pvt->_debug;
}


const char *sqlrauth::auth(sqlrserverconnection *sqlrcon,
					sqlrcredentials *cred) {
	return NULL;
}
