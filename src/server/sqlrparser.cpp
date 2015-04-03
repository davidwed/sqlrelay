// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrparser::sqlrparser() {
}

sqlrparser::~sqlrparser() {
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

bool sqlrparser::write(xmldomnode *node,
				stringbuffer *output,
				bool omitsiblings) {
	return false;
}

bool sqlrparser::write(xmldomnode *node, stringbuffer *output) {
	return false;
}
