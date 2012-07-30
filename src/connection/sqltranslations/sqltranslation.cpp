// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltranslation.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>

sqltranslation::sqltranslation(sqltranslations *sqlts,
				xmldomnode *parameters) {
	this->sqlts=sqlts;
	this->parameters=parameters;
	dl=NULL;
}

sqltranslation::~sqltranslation() {
	if (dl) {
		dl->close();
		delete dl;
	}
}

void sqltranslation::attachModule(dynamiclib *dl) {
	this->dl=dl;
}

bool sqltranslation::run(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				xmldom *querytree) {
	return true;
}
