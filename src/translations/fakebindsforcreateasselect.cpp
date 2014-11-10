// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrservercontroller.h>
#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlreparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class fakebindsforcreateasselect : public sqlrtranslation {
	public:
			fakebindsforcreateasselect(
						sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldom *querytree);
};

fakebindsforcreateasselect::fakebindsforcreateasselect(
						sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool fakebindsforcreateasselect::usesTree() {
	return true;
}

bool fakebindsforcreateasselect::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	debugFunction();

	if (!querytree->getRootNode()->
				getFirstTagChild(sqlreparser::_create)->
				getFirstTagChild(sqlreparser::_table)->
				getFirstTagChild(sqlreparser::_as)->
				getNextTagSibling(sqlreparser::_select)->
				isNullNode()) {
		sqlrcur->fakeinputbindsforthisquery=true;
	}
	return true;
}

extern "C" {
	sqlrtranslation	*new_fakebindsforcreateasselect(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new fakebindsforcreateasselect(sqlts,parameters,debug);
	}
}
