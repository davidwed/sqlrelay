// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrnotification::sqlrnotification(sqlrnotifications *ns,
					xmldomnode *parameters,
					bool debug) {
	this->ns=ns;
	this->parameters=parameters;
	this->debug=debug;
}

sqlrnotification::~sqlrnotification() {
}

bool sqlrnotification::run(sqlrlistener *sqlrl,
			sqlrserverconnection *sqlrcon,
			sqlrservercursor *sqlrcur,
			sqlrevent_t event,
			const char *info) {
	return true;
}
