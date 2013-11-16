// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#include <sqlrprotocol.h>

sqlrprotocol::sqlrprotocol(sqlrcontroller_svr *cont,
				sqlrconnection_svr *conn,
				sqlrconfigfile *cfgfl,
				filedescriptor *clientsock) {
	this->cont=cont;
	this->conn=conn;
	this->cfgfl=cfgfl;
	this->clientsock=clientsock;
}

sqlrprotocol::~sqlrprotocol() {
}
