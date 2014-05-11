// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <sqlrauth.h>
#include <rudiments/charstring.h>

sqlrauth::sqlrauth(xmldomnode *usersnode, sqlrpwdencs *sqlrpe) {
	this->usersnode=usersnode;
	this->sqlrpe=sqlrpe;
}

sqlrauth::~sqlrauth() {
}

bool sqlrauth::authenticate(const char *user, const char *password) {
	return NULL;
}
