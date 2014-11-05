// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrtranslation.h>
#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>

sqlrtranslation::sqlrtranslation(sqlrtranslations *sqlts,
				xmldomnode *parameters,
				bool debug) {
	this->sqlts=sqlts;
	this->parameters=parameters;
	this->debug=debug;
}

sqlrtranslation::~sqlrtranslation() {
}

bool sqlrtranslation::run(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				xmldom *querytree) {
	return true;
}
