// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrnotification::sqlrnotification(sqlrnotifications *ns,
					xmldomnode *parameters) {
	this->ns=ns;
	this->parameters=parameters;
}

sqlrnotification::~sqlrnotification() {
}

bool sqlrnotification::init(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon) {
	return true;
}

bool sqlrnotification::run(sqlrlistener *sqlrl,
			sqlrserverconnection *sqlrcon,
			sqlrservercursor *sqlrcur,
			sqlrevent_t event,
			const char *info) {
	return true;
}
