// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrrouter::sqlrrouter(xmldomnode *parameters, bool debug) {
	this->parameters=parameters;
	this->debug=debug;
}

sqlrrouter::~sqlrrouter() {
}

const char *sqlrrouter::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur) {
	return NULL;
}
