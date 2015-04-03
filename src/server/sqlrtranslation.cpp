// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrtranslation::sqlrtranslation(sqlrtranslations *sqlts,
				xmldomnode *parameters,
				bool debug) {
	this->sqlts=sqlts;
	this->parameters=parameters;
	this->debug=debug;
}

sqlrtranslation::~sqlrtranslation() {
}

bool sqlrtranslation::usesTree() {
	return false;
}

bool sqlrtranslation::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				const char *query,
				stringbuffer *translatedquery) {
	return true;
}

bool sqlrtranslation::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				xmldom *querytree) {
	return true;
}
