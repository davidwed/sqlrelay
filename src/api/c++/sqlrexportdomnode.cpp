// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportdomnode.h>

sqlrexportdomnode::sqlrexportdomnode() : sqlrexport() {
	topdomnode=NULL;
	columnsdomnode=NULL;
	currentcolumndomnode=NULL;
	rowsdomnode=NULL;
	currentrowdomnode=NULL;
	currentfielddomnode=NULL;
}

sqlrexportdomnode::~sqlrexportdomnode() {
}

void sqlrexportdomnode::setDomNode(domnode *dn) {
	topdomnode=dn;
}

domnode	*sqlrexportdomnode::getDomNode() {
	return topdomnode;
}

void sqlrexportdomnode::setColumnsDomNode(domnode *dn) {
	columnsdomnode=dn;
}

domnode	*sqlrexportdomnode::getColumnsDomNode() {
	return columnsdomnode;
}

void sqlrexportdomnode::setCurrentColumnDomNode(domnode *dn) {
	currentcolumndomnode=dn;
}

domnode	*sqlrexportdomnode::getCurrentColumnDomNode() {
	return currentcolumndomnode;
}

void sqlrexportdomnode::setRowsDomNode(domnode *dn) {
	rowsdomnode=dn;
}

domnode	*sqlrexportdomnode::getRowsDomNode() {
	return rowsdomnode;
}

void sqlrexportdomnode::setCurrentRowDomNode(domnode *dn) {
	currentrowdomnode=dn;
}

domnode	*sqlrexportdomnode::getCurrentRowDomNode() {
	return currentrowdomnode;
}

void sqlrexportdomnode::setCurrentFieldDomNode(domnode *dn) {
	currentfielddomnode=dn;
}

domnode	*sqlrexportdomnode::getCurrentFieldDomNode() {
	return currentfielddomnode;
}

bool sqlrexportdomnode::exportData() {
	return sqlrexport::exportData();
}

void sqlrexportdomnode::clearFlagsAndCounts() {
	sqlrexport::clearFlagsAndCounts();
	columnsdomnode=NULL;
	currentcolumndomnode=NULL;
	rowsdomnode=NULL;
	currentrowdomnode=NULL;
	currentfielddomnode=NULL;
}

bool sqlrexportdomnode::sanityCheck() {

	if (!sqlrexport::sanityCheck()) {
		return false;
	}

	if (!getDomNode()) {
		return error(0,"No domnode set with setDomNode()");
	}
	return true;
}
