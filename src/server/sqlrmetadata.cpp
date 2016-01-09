// Copyright (c) 1999-2014  David Muse
// All rights reserved

#include <sqlrelay/sqlrserver.h>

sqlrmetadata::sqlrmetadata(sqlrservercontroller *cont,
				xmldomnode *parameters, bool debug) {
	this->cont=cont;
	this->parameters=parameters;
	this->debug=debug;
	query=NULL;
	tree=NULL;
	mdpool=new memorypool(0,128,100);
	this->dirty=false;
}

sqlrmetadata::~sqlrmetadata() {
	delete mdpool;
}

void sqlrmetadata::setQuery(const char *query) {
	reset();
	dirty=true;
	this->query=query;
}

void sqlrmetadata::setQueryTree(xmldom *tree) {
	reset();
	dirty=true;
	this->tree=tree;
}

void sqlrmetadata::reset() {
	if (dirty) {
		this->query=NULL;
		this->tree=NULL;
		columnsofaliases.clear();
		aliasesofcolumns.clear();
		tablesofaliases.clear();
		aliasesoftables.clear();
		tablesofcolumns.clear();
		mdpool->deallocate();
		dirty=false;
	}
}

void sqlrmetadata::collect() {
	if (dirty) {
		collectMetaData();
		dirty=false;
	}
}

void sqlrmetadata::collectMetaData() {
	// by default, do nothing...
}

dictionary< const char *, const char * > *sqlrmetadata::getColumnsOfAliases() {
	collect();
	return &columnsofaliases;
}

dictionary< const char *, const char * > *sqlrmetadata::getAliasesOfColumns() {
	collect();
	return &aliasesofcolumns;
}

dictionary< const char *, const char * > *sqlrmetadata::getTablesOfAliases() {
	collect();
	return &tablesofaliases;
}

dictionary< const char *, const char * > *sqlrmetadata::getAliasesOfTables() {
	collect();
	return &aliasesoftables;
}

dictionary< const char *, const char * > *sqlrmetadata::getTablesOfColumns() {
	collect();
	return &tablesofcolumns;
}
