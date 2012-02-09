// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltrigger.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>

sqltrigger::sqltrigger(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqltrigger::~sqltrigger() {
}

bool sqltrigger::run(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				xmldom *querytree,
				bool before,
				bool success) {
	return true;
}
