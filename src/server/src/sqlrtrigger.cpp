// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrtrigger.h>
#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>

sqlrtrigger::sqlrtrigger(xmldomnode *parameters, bool debug) {
	this->parameters=parameters;
	this->debug=debug;
}

sqlrtrigger::~sqlrtrigger() {
}

bool sqlrtrigger::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				xmldom *querytree,
				bool before,
				bool success) {
	return true;
}
