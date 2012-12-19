// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRLOGGER_H
#define SQLRLOGGER_H

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

enum sqlrlogger_loglevel_t {
	SQLRLOGGER_LOGLEVEL_INFO=0,
	SQLRLOGGER_LOGLEVEL_WARNING,
	SQLRLOGGER_LOGLEVEL_ERROR
};

enum sqlrlogger_eventtype_t {
	SQLRLOGGER_EVENTTYPE_CLI_CONNECTED=0,
	SQLRLOGGER_EVENTTYPE_CLI_CONNECTION_REFUSED,
	SQLRLOGGER_EVENTTYPE_CLI_DISCONNECTED,
	SQLRLOGGER_EVENTTYPE_CLI_SOCKET_ERROR,
	SQLRLOGGER_EVENTTYPE_DB_CONNECTED,
	SQLRLOGGER_EVENTTYPE_DB_DISCONNECTED,
	SQLRLOGGER_EVENTTYPE_DB_SOCKET_ERROR,
	SQLRLOGGER_EVENTTYPE_DB_ERROR,
	SQLRLOGGER_EVENTTYPE_SQLR_QUERY,
	SQLRLOGGER_EVENTTYPE_SQLR_INTERNAL
};

class sqlrlogger {
	public:
			sqlrlogger(rudiments::xmldomnode *parameters);
		virtual	~sqlrlogger();

		virtual bool	init(sqlrconnection_svr *sqlrcon);
		virtual bool	run(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						sqlrlogger_loglevel_t level,
						sqlrlogger_eventtype_t event,
						const char *info);
	protected:
		const char	*logLevel(sqlrlogger_loglevel_t level);
		const char	*eventType(sqlrlogger_eventtype_t event);
		rudiments::xmldomnode	*parameters;
};

#endif
