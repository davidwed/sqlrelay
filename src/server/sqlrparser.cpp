// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrparserprivate {
	friend class sqlrparser;
	private:
		xmldom	*_qtree;
		bool	_foreignqtree;
		xmldom	*_mdtree;
		bool	_foreignmdtree;
};

sqlrparser::sqlrparser(sqlrservercontroller *cont, domnode *parameters) :
					sqlrservermodule(cont,parameters) {
	pvt=new sqlrparserprivate;
	pvt->_qtree=NULL;
	pvt->_foreignqtree=false;
	pvt->_mdtree=NULL;
	pvt->_foreignmdtree=false;
}

sqlrparser::~sqlrparser() {
	delete pvt->_qtree;
	delete pvt->_mdtree;
	delete pvt;
}

void sqlrparser::setQueryTree(xmldom *qtree) {
	if (!pvt->_foreignqtree) {
		delete pvt->_qtree;
	}
	pvt->_foreignqtree=qtree;
	pvt->_qtree=qtree;
}

xmldom *sqlrparser::getQueryTree() {
	return pvt->_qtree;
}

xmldom *sqlrparser::detachQueryTree() {
	xmldom	*retval=pvt->_qtree;
	pvt->_qtree=NULL;
	pvt->_foreignqtree=false;
	return retval;
}

void sqlrparser::setMetaDataTree(xmldom *mdtree) {
	if (!pvt->_foreignmdtree) {
		delete pvt->_mdtree;
	}
	pvt->_foreignmdtree=mdtree;
	pvt->_mdtree=mdtree;
}

xmldom *sqlrparser::getMetaDataTree() {
	return pvt->_mdtree;
}

xmldom *sqlrparser::detachMetaDataTree() {
	xmldom	*retval=pvt->_mdtree;
	pvt->_mdtree=NULL;
	pvt->_foreignmdtree=false;
	return retval;
}

bool sqlrparser::parse(const char *query) {
	if (!pvt->_foreignqtree) {
		delete pvt->_qtree;
		pvt->_qtree=new xmldom(false);
		pvt->_qtree->createRootNode();
	}
	if (!pvt->_foreignmdtree) {
		delete pvt->_mdtree;
		pvt->_mdtree=new xmldom(false);
		pvt->_mdtree->createRootNode();
	}
	return true;
}

bool sqlrparser::write(stringbuffer *output) {
	// by default, do nothing...
	return false;
}

bool sqlrparser::write(domnode *node,
				stringbuffer *output,
				bool omitsiblings) {
	// by default, do nothing...
	return false;
}

bool sqlrparser::write(domnode *node, stringbuffer *output) {
	// by default, do nothing...
	return false;
}
