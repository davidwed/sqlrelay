// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRLOGGERS_H
#define SQLRLOGGERS_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/singlylinkedlist.h>
#include <rudiments/dynamiclib.h>
#include <sqlrelay/sqlrlogger.h>

class sqlrlistener;
class sqlrserverconnection;
class sqlrservercursor;

class SQLRSERVER_DLLSPEC sqlrloggerplugin {
	public:
		sqlrlogger	*lg;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrloggers {
	public:
			sqlrloggers();
			~sqlrloggers();

		bool	loadLoggers(const char *loggers);
		void	initLoggers(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon);
		void	runLoggers(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrlogger_eventtype_t event,
					const char *info);
	private:
		void		unloadLoggers();
		void		loadLogger(xmldomnode *logger);

		xmldom					*xmld;
		singlylinkedlist< sqlrloggerplugin * >	llist;
};

#endif
