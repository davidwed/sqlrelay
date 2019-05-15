// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrparserprivate {
	friend class sqlrparser;
	private:
		domnode		*_parameters;
};

sqlrparser::sqlrparser(sqlrservercontroller *cont, domnode *parameters) {
	pvt=new sqlrparserprivate;
	pvt->_parameters=parameters;
}

sqlrparser::~sqlrparser() {
	delete pvt;
}

bool sqlrparser::parse(const char *query) {
	return false;
}

void sqlrparser::useTree(xmldom *tree) {
}

xmldom *sqlrparser::getTree() {
	return NULL;
}

xmldom *sqlrparser::detachTree() {
	return NULL;
}

bool sqlrparser::write(stringbuffer *output) {
	return false;
}

bool sqlrparser::write(domnode *node,
				stringbuffer *output,
				bool omitsiblings) {
	return false;
}

bool sqlrparser::write(domnode *node, stringbuffer *output) {
	return false;
}

void sqlrparser::getMetaData(domnode *node) {
	// by default, do nothing...
}

domnode *sqlrparser::getParameters() {
	return pvt->_parameters;
}

void sqlrparser::endSession() {
	// nothing for now, maybe in the future
}
