// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

class sqlrauthprivate {
	friend class sqlrauth;
	public:
		sqlrauths	*_auths;
		sqlrpwdencs	*_sqlrpe;
		domnode	*_parameters;
};

sqlrauth::sqlrauth(sqlrservercontroller *cont,
				sqlrauths *auths,
				sqlrpwdencs *sqlrpe,
				domnode *parameters) {
	pvt=new sqlrauthprivate;
	this->cont=cont;
	pvt->_auths=auths;
	pvt->_sqlrpe=sqlrpe;
	pvt->_parameters=parameters;
}

sqlrauth::~sqlrauth() {
	delete pvt;
}

const char *sqlrauth::auth(sqlrcredentials *cred) {
	return NULL;
}

sqlrauths *sqlrauth::getAuths() {
	return pvt->_auths;
}

sqlrpwdencs *sqlrauth::getPasswordEncryptions() {
	return pvt->_sqlrpe;
}

domnode *sqlrauth::getParameters() {
	return pvt->_parameters;
}
