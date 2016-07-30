// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrlogger::sqlrlogger(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrlogger::~sqlrlogger() {
}

bool sqlrlogger::init(sqlrlistener *sqlrl, sqlrserverconnection *sqlrcon) {
	return true;
}

bool sqlrlogger::run(sqlrlistener *sqlrl,
			sqlrserverconnection *sqlrcon,
			sqlrservercursor *sqlrcur,
			sqlrlogger_loglevel_t level,
			sqlrevent_t event,
			const char *info) {
	return true;
}

static const char *loglevels[]={"DEBUG","INFO","WARNING","ERROR"};

const char *sqlrlogger::logLevel(sqlrlogger_loglevel_t level) {
	return loglevels[(uint8_t)level];
}

// FIXME: push up and consolidate
static const char *eventtypes[]={
	"CLIENT_CONNECTED",
	"CLIENT_CONNECTION_REFUSED",
	"CLIENT_DISCONNECTED",
	"CLIENT_PROTOCOL_ERROR",
	"DB_LOGIN",
	"DB_LOGOUT",
	"DB_ERROR",
	"DB_WARNING",
	"QUERY",
	"INTERNAL_ERROR",
	"INTERNAL_WARNING",
	"DEBUG_MESSAGE"
};

const char *sqlrlogger::eventType(sqlrevent_t event) {
	return eventtypes[(uint16_t)event];
}
