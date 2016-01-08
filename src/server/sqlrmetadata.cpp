// Copyright (c) 1999-2014  David Muse
// All rights reserved

#include <sqlrelay/sqlrserver.h>

sqlrmetadata::sqlrmetadata(sqlrservercontroller *cont, bool debug) {
	this->cont=cont;
	this->debug=debug;
	this->query=NULL;
	this->tree=NULL;
}

sqlrmetadata::~sqlrmetadata() {
}

void sqlrmetadata::setQuery(const char *query) {
	this->query=query;
	this->tree=NULL;
}

void sqlrmetadata::setQueryTree(xmldom *tree) {
	this->query=NULL;
	this->tree=tree;
}

const char *sqlrmetadata::getColumnOfAlias(const char *alias) {
	return NULL;
}

const char *sqlrmetadata::getAliasOfColumn(const char *column) {
	return NULL;
}

const char *sqlrmetadata::getTableOfAlias(const char *alias) {
	return NULL;
}

const char *sqlrmetadata::getAliasOfTable(const char *table) {
	return NULL;
}

const char *sqlrmetadata::getTableOfColumn(const char *column) {
	return NULL;
}
