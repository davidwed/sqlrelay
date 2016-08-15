// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

sqlrauth::sqlrauth(xmldomnode *parameters, sqlrpwdencs *sqlrpe, bool debug) {
	this->parameters=parameters;
	this->sqlrpe=sqlrpe;
	this->debug=debug;
}

sqlrauth::~sqlrauth() {
}

void sqlrauth::setParameter(const char *name, const char *value) {
}

bool sqlrauth::auth(sqlrserverconnection *sqlrcon,
			const char *user, const char *password) {
	return false;
}

bool sqlrauth::auth(sqlrserverconnection *sqlrcon,
			const char *user, const char *password,
			const char *method, const char *salt) {
	return auth(sqlrcon,user,password);
}

const char *sqlrauth::auth(sqlrserverconnection *sqlrcon,
					sqlrcredentials *cred) {
	return NULL;
}
