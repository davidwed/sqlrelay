// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrschedule::sqlrschedule(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrschedule::~sqlrschedule() {
}

bool sqlrschedule::init(sqlrserverconnection *sqlrcon) {
	return true;
}

bool sqlrschedule::run(sqlrserverconnection *sqlrcon) {
	return true;
}
