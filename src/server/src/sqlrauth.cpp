// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

sqlrauth::sqlrauth(xmldomnode *parameters, sqlrpwdencs *sqlrpe) {
	this->parameters=parameters;
	this->sqlrpe=sqlrpe;
}

sqlrauth::~sqlrauth() {
}

bool sqlrauth::authenticate(const char *user, const char *password) {
	return false;
}
