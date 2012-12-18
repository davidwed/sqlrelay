// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRLOGGERS_H
#define SQLRLOGGERS_H

#include <rudiments/xmldom.h>
#include <rudiments/linkedlist.h>
#include <rudiments/dynamiclib.h>
#include <sqlrlogger.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class sqlrloggerplugin {
	public:
		sqlrlogger		*lg;
		rudiments::dynamiclib	*dl;
};

class sqlrloggers {
	public:
			sqlrloggers();
			~sqlrloggers();

		bool	loadLoggers(const char *loggers);
		void	initLoggers(sqlrconnection_svr *sqlrcon);
		void	runLoggers(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						sqlrlogger_loglevel_t level,
						sqlrlogger_eventtype_t event);
	private:
		void		unloadLoggers();
		void		loadLogger(rudiments::xmldomnode *logger);

		rudiments::xmldom				*xmld;
		rudiments::linkedlist< sqlrloggerplugin * >	llist;
};

#endif
