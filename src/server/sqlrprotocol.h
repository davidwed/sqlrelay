// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#ifndef SQLRPROTOCOL_H
#define SQLRPROTOCOL_H

#include <config.h>
#include <defaults.h>
#include <rudiments/filedescriptor.h>

enum sqlrprotocol_t {
	SQLRPROTOCOL_SQLRCLIENT=0,
	SQLRPROTOCOL_HTTP,
	SQLRPROTOCOL_MYSQL,
	SQLRPROTOCOL_POSTGRESQL,
	SQLRPROTOCOL_TDS,
	SQLRPROTOCOL_UNKNOWN
};

class sqlrcontroller_svr;
class sqlrconnection_svr;
class sqlrconfigfile;

class sqlrprotocol {
	public:
			sqlrprotocol(sqlrcontroller_svr *cont,
					sqlrconnection_svr *conn,
					sqlrconfigfile *cfgfl,
					filedescriptor *clientsock);
		virtual	~sqlrprotocol();

		virtual void	clientSession()=0;

	protected:
		sqlrcontroller_svr	*cont;
		sqlrconnection_svr	*conn;
		sqlrconfigfile		*cfgfl;
		filedescriptor		*clientsock;
};

#endif
