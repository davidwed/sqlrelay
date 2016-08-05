// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrrouter::sqlrrouter(xmldomnode *parameters, bool debug) {
	this->parameters=parameters;
	this->debug=debug;
}

sqlrrouter::~sqlrrouter() {
}

bool sqlrrouter::init(sqlrserverconnection *sqlrcon) {
	return true;
}

const char *sqlrrouter::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur) {
	return NULL;
}
