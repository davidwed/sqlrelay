// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrnotification::sqlrnotification(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrnotification::~sqlrnotification() {
}

bool sqlrnotification::init(sqlrlistener *sqlrl, sqlrserverconnection *sqlrcon) {
	return true;
}

bool sqlrnotification::run(sqlrlistener *sqlrl,
			sqlrserverconnection *sqlrcon,
			sqlrservercursor *sqlrcur,
			sqlrevent_t event,
			const char *info) {
	return true;
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
	"DEBUG_MESSAGE",
	"SCHEDULE_VIOLATION"
};

const char *sqlrnotification::eventType(sqlrevent_t event) {
	return eventtypes[(uint16_t)event];
}
