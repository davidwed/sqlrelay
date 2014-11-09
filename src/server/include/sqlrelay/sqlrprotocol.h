// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#ifndef SQLRPROTOCOL_H
#define SQLRPROTOCOL_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <rudiments/filedescriptor.h>

enum sqlrclientexitstatus_t {
	SQLRCLIENTEXITSTATUS_ERROR=0,
	SQLRCLIENTEXITSTATUS_CLOSED_CONNECTION,
	SQLRCLIENTEXITSTATUS_ENDED_SESSION,
	SQLRCLIENTEXITSTATUS_SUSPENDED_SESSION
};

class sqlrservercontroller;
class sqlrserverconnection;
class sqlrconfigfile;

class SQLRSERVER_DLLSPEC sqlrprotocol {
	public:
			sqlrprotocol(sqlrservercontroller *cont,
					sqlrserverconnection *conn);
		virtual	~sqlrprotocol();

		void	setClientSocket(filedescriptor *clientsock);

		virtual sqlrclientexitstatus_t	clientSession()=0;

	protected:
		sqlrservercontroller	*cont;
		sqlrserverconnection	*conn;
		filedescriptor		*clientsock;
};

#endif
