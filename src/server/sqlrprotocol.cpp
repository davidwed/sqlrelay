// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrprotocol::sqlrprotocol(sqlrservercontroller *cont) {
	this->cont=cont;
	this->clientsock=NULL;
}

sqlrprotocol::~sqlrprotocol() {
}

void sqlrprotocol::setClientSocket(filedescriptor *clientsock) {
	this->clientsock=clientsock;
}
