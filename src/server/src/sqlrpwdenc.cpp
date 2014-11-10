// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

sqlrpwdenc::sqlrpwdenc(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrpwdenc::~sqlrpwdenc() {
}

const char *sqlrpwdenc::getId() {
	return parameters->getAttributeValue("id");
}

bool sqlrpwdenc::oneWay() {
	return false;
}

char *sqlrpwdenc::encrypt(const char *value) {
	return NULL;
}

char *sqlrpwdenc::decrypt(const char *value) {
	return NULL;
}
