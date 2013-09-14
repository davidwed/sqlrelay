// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRLOGGER_H
#define SQLRLOGGER_H

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrlistener;
class sqlrconnection_svr;
class sqlrcursor_svr;

enum sqlrlogger_loglevel_t {
	SQLRLOGGER_LOGLEVEL_DEBUG=0,
	SQLRLOGGER_LOGLEVEL_INFO,
	SQLRLOGGER_LOGLEVEL_WARNING,
	SQLRLOGGER_LOGLEVEL_ERROR
};

enum sqlrlogger_eventtype_t {
	SQLRLOGGER_EVENTTYPE_CLIENT_CONNECTED=0,
	SQLRLOGGER_EVENTTYPE_CLIENT_CONNECTION_REFUSED,
	SQLRLOGGER_EVENTTYPE_CLIENT_DISCONNECTED,
	SQLRLOGGER_EVENTTYPE_CLIENT_PROTOCOL_ERROR,
	SQLRLOGGER_EVENTTYPE_DB_LOGIN,
	SQLRLOGGER_EVENTTYPE_DB_LOGOUT,
	SQLRLOGGER_EVENTTYPE_DB_ERROR,
	SQLRLOGGER_EVENTTYPE_QUERY,
	SQLRLOGGER_EVENTTYPE_INTERNAL_ERROR,
	SQLRLOGGER_EVENTTYPE_DEBUG_MESSAGE
};

class sqlrlogger {
	public:
			sqlrlogger(xmldomnode *parameters);
		virtual	~sqlrlogger();

		virtual bool	init(sqlrlistener *sqlrl,
					sqlrconnection_svr *sqlrcon);
		virtual bool	run(sqlrlistener *sqlrl,
					sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrlogger_eventtype_t event,
					const char *info);
	protected:
		const char	*logLevel(sqlrlogger_loglevel_t level);
		const char	*eventType(sqlrlogger_eventtype_t event);
		xmldomnode	*parameters;
};

#endif
