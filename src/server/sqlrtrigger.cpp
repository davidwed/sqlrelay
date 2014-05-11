// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrtrigger.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>

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
