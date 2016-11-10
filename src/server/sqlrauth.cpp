// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

class sqlrauthprivate {
	friend class sqlrauth;
	public:
		sqlrpwdencs	*_sqlrpe;
		xmldomnode	*_parameters;
};

sqlrauth::sqlrauth(sqlrservercontroller *cont, sqlrpwdencs *sqlrpe,
						xmldomnode *parameters) {
	pvt=new sqlrauthprivate;
	this->cont=cont;
	pvt->_parameters=parameters;
	pvt->_sqlrpe=sqlrpe;
}

sqlrauth::~sqlrauth() {
	delete pvt;
}

const char *sqlrauth::auth(sqlrcredentials *cred) {
	return NULL;
}

sqlrpwdencs *sqlrauth::getPasswordEncryptions() {
	return pvt->_sqlrpe;
}

xmldomnode *sqlrauth::getParameters() {
	return pvt->_parameters;
}
