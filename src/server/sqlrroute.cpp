// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrroute::sqlrroute(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrroute::~sqlrroute() {
}

bool sqlrroute::init(sqlrserverconnection *sqlrcon) {
	return true;
}

bool sqlrroute::route(sqlrserverconnection *sqlrcon) {
	return true;
}
