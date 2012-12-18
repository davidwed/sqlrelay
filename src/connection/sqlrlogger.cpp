// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrlogger.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

sqlrlogger::sqlrlogger(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrlogger::~sqlrlogger() {
}

bool sqlrlogger::init(sqlrconnection_svr *sqlrcon) {
	return true;
}

bool sqlrlogger::run(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				sqlrlogger_loglevel_t level,
				sqlrlogger_eventtype_t event) {
	return true;
}

static const char *loglevels[]={"INFO","WARNING","ERROR"};

const char *sqlrlogger::logLevel(sqlrlogger_loglevel_t level) {
	return loglevels[(uint8_t)level];
}

static const char *eventtypes[]={
	"CLI_CONNECTED",
	"CLI_CONNECTION_REFUSED",
	"CLI_DISCONNECTED",
	"CLI_SOCKET_ERROR",
	"DB_CONNECTED",
	"DB_DISCONNECTED",
	"DB_SOCKET_ERROR",
	"DB_ERROR",
	"SQLR_COMMAND_COMPLETED",
	"SQLR_INTERNAL"
};

const char *sqlrlogger::eventType(sqlrlogger_eventtype_t event) {
	return eventtypes[(uint16_t)event];
}
