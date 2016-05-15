// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrprotocol::sqlrprotocol(sqlrservercontroller *cont,
				xmldomnode *parameters,
				bool debug) {
	this->cont=cont;
	this->parameters=parameters;
	this->debug=debug;
	this->clientsock=NULL;
}

sqlrprotocol::~sqlrprotocol() {
}

void sqlrprotocol::setClientSocket(filedescriptor *clientsock) {
	this->clientsock=clientsock;
}

gsscontext *sqlrprotocol::getGSSContext() {
	return &gctx;
}

tlscontext *sqlrprotocol::getTLSContext() {
	return &tctx;
}
