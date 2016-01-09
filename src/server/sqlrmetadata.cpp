// Copyright (c) 1999-2014  David Muse
// All rights reserved

#include <sqlrelay/sqlrserver.h>

sqlrmetadata::sqlrmetadata(sqlrservercontroller *cont,
				xmldomnode *parameters, bool debug) {
	this->cont=cont;
	this->parameters=parameters;
	this->debug=debug;
	tree=NULL;
}

sqlrmetadata::~sqlrmetadata() {
}

void sqlrmetadata::setQueryTree(xmldom *tree) {
	this->tree=tree;
}

void sqlrmetadata::collectMetaData() {
	// by default, do nothing...
}
