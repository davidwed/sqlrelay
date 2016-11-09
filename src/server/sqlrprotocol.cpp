// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrprotocolprivate {
	friend class sqlrprotocol;
	private:
		xmldomnode		*_parameters;
		bool			_debug;
};

sqlrprotocol::sqlrprotocol(sqlrservercontroller *cont,
				xmldomnode *parameters,
				bool debug) {
	pvt=new sqlrprotocolprivate;
	this->cont=cont;
	pvt->_parameters=parameters;
	pvt->_debug=debug;
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

bool sqlrprotocol::getDebug() {
	return pvt->_debug;
}
