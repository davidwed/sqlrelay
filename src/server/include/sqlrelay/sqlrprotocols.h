// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#ifndef SQLRPROTOCOLS_H
#define SQLRPROTOCOLS_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <rudiments/dynamiclib.h>
#include <rudiments/dictionary.h>
#include <sqlrelay/sqlrprotocol.h>

class SQLRSERVER_DLLSPEC sqlrprotocolplugin {
	public:
		sqlrprotocol	*pr;
		dynamiclib	*dl;
};

class sqlrservercontroller;
class sqlrserverconnection;

class SQLRSERVER_DLLSPEC sqlrprotocols {
	public:
			sqlrprotocols(sqlrservercontroller *cont,
					sqlrserverconnection *conn);
			~sqlrprotocols();

		bool		loadProtocols();
		sqlrprotocol	*getProtocol(const char *module);
	private:
		void	unloadProtocols();
		void	loadProtocol(const char *module);

		dictionary< const char *, sqlrprotocolplugin * >	protos;

		sqlrservercontroller	*cont;
		sqlrserverconnection	*conn;
};

#endif
