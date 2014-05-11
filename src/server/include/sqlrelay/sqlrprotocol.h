// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#ifndef SQLRPROTOCOL_H
#define SQLRPROTOCOL_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <rudiments/filedescriptor.h>

#define	SQLRPROTOCOLCOUNT 5
enum sqlrprotocol_t {
	SQLRPROTOCOL_SQLRCLIENT=0,
	SQLRPROTOCOL_HTTP,
	SQLRPROTOCOL_MYSQL,
	SQLRPROTOCOL_POSTGRESQL,
	SQLRPROTOCOL_TDS,
	SQLRPROTOCOL_UNKNOWN
};

enum sqlrclientexitstatus_t {
	SQLRCLIENTEXITSTATUS_ERROR=0,
	SQLRCLIENTEXITSTATUS_CLOSED_CONNECTION,
	SQLRCLIENTEXITSTATUS_ENDED_SESSION,
	SQLRCLIENTEXITSTATUS_SUSPENDED_SESSION
};

class sqlrcontroller_svr;
class sqlrconnection_svr;
class sqlrconfigfile;

class SQLRSERVER_DLLSPEC sqlrprotocol {
	public:
			sqlrprotocol(sqlrcontroller_svr *cont,
					sqlrconnection_svr *conn,
					sqlrconfigfile *cfgfl);
		virtual	~sqlrprotocol();

		void	setClientSocket(filedescriptor *clientsock);

		virtual sqlrclientexitstatus_t	clientSession()=0;
		virtual	void			closeClientSession()=0;

	protected:
		sqlrcontroller_svr	*cont;
		sqlrconnection_svr	*conn;
		sqlrconfigfile		*cfgfl;
		filedescriptor		*clientsock;
};

#endif
