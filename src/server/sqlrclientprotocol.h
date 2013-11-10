// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#ifndef SQLRCLIENTPROTOCOL_H
#define SQLRCLIENTPROTOCOL_H

#include <sqlrprotocol.h>

class sqlrclientprotocol : public sqlrprotocol {
	public:
			sqlrclientprotocol(sqlrcontroller_svr *cont,
						sqlrconnection_svr *conn,
						filedescriptor *clientsock);
		virtual	~sqlrclientprotocol();

		void	clientSession();
};

#endif
