// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRLOGGERS_H
#define SQLRLOGGERS_H

#include <sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/linkedlist.h>
#include <rudiments/dynamiclib.h>
#include <sqlrlogger.h>

class sqlrlistener;
class sqlrconnection_svr;
class sqlrcursor_svr;

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
					sqlrconnection_svr *sqlrcon);
		void	runLoggers(sqlrlistener *sqlrl,
					sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrlogger_eventtype_t event,
					const char *info);
	private:
		void		unloadLoggers();
		void		loadLogger(xmldomnode *logger);

		xmldom					*xmld;
		linkedlist< sqlrloggerplugin * >	llist;
};

#endif
