// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrprotocolprivate {
	friend class sqlrprotocol;
	private:
		xmldomnode		*_parameters;
};

sqlrprotocol::sqlrprotocol(sqlrservercontroller *cont,
				xmldomnode *parameters) {
	pvt=new sqlrprotocolprivate;
	this->cont=cont;
	pvt->_parameters=parameters;
}

sqlrprotocol::~sqlrprotocol() {
	delete pvt;
}

gsscontext *sqlrprotocol::getGSSContext() {
	return NULL;
}

tlscontext *sqlrprotocol::getTLSContext() {
	return NULL;
}

xmldomnode *sqlrprotocol::getParameters() {
	return pvt->_parameters;
}
