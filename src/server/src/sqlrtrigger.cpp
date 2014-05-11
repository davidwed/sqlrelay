// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrtrigger.h>
#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>

sqlrtrigger::sqlrtrigger(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrtrigger::~sqlrtrigger() {
}

bool sqlrtrigger::run(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				xmldom *querytree,
				bool before,
				bool success) {
	return true;
}
