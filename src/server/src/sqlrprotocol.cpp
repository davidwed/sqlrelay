// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrprotocol.h>

sqlrprotocol::sqlrprotocol(sqlrcontroller_svr *cont,
				sqlrconnection_svr *conn) {
	this->cont=cont;
	this->conn=conn;
	this->clientsock=NULL;
}

sqlrprotocol::~sqlrprotocol() {
}

void sqlrprotocol::setClientSocket(filedescriptor *clientsock) {
	this->clientsock=clientsock;
}
