// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrtranslation.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>

sqlrtranslation::sqlrtranslation(sqlrtranslations *sqlts,
				xmldomnode *parameters) {
	this->sqlts=sqlts;
	this->parameters=parameters;
}

sqlrtranslation::~sqlrtranslation() {
}

bool sqlrtranslation::run(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				xmldom *querytree) {
	return true;
}
