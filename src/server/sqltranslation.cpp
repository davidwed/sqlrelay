// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltranslation.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>

sqltranslation::sqltranslation(sqltranslations *sqlts,
				xmldomnode *parameters) {
	this->sqlts=sqlts;
	this->parameters=parameters;
}

sqltranslation::~sqltranslation() {
}

bool sqltranslation::run(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				xmldom *querytree) {
	return true;
}
