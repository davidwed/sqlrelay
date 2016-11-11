// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrprotocolprivate {
	friend class sqlrprotocol;
	private:
		sqlrprotocols		*_ps;
		xmldomnode		*_parameters;
};

sqlrprotocol::sqlrprotocol(sqlrservercontroller *cont,
				sqlrprotocols *ps,
				xmldomnode *parameters) {
	pvt=new sqlrprotocolprivate;
	this->cont=cont;
	pvt->_ps=ps;
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

sqlrprotocols *sqlrprotocol::getProtocols() {
	return pvt->_ps;
}

xmldomnode *sqlrprotocol::getParameters() {
	return pvt->_parameters;
}
