// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#include <sqlrprotocol.h>

sqlrprotocol::sqlrprotocol(sqlrcontroller_svr *cont,
				sqlrconnection_svr *conn,
				filedescriptor *clientsock) {
	this->cont=cont;
	this->conn=conn;
	this->clientsock=clientsock;
}

sqlrprotocol::~sqlrprotocol() {
}
