// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrrouter::sqlrrouter(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrrouter::~sqlrrouter() {
}

bool sqlrrouter::init(sqlrserverconnection *sqlrcon) {
	return true;
}

bool sqlrrouter::route(sqlrserverconnection *sqlrcon) {
	return true;
}
