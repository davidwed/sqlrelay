// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrcontroller.h>
#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class fakebindsforcreateasselect : public sqlrtranslation {
	public:
			fakebindsforcreateasselect(
						sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	run(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldom *querytree);
};

fakebindsforcreateasselect::fakebindsforcreateasselect(
						sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool fakebindsforcreateasselect::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();

	if (!querytree->getRootNode()->
				getFirstTagChild(sqlparser::_create)->
				getFirstTagChild(sqlparser::_table)->
				getFirstTagChild(sqlparser::_as)->
				getNextTagSibling(sqlparser::_select)->
				isNullNode()) {
		sqlrcon->cont->setFakeInputBindsForThisQuery(sqlrcur,true);
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
