// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqlrtranslation.h>
#include <debugprint.h>

class fakebindsforcreateasselect : public sqlrtranslation {
	public:
			fakebindsforcreateasselect(
						sqlrtranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldom *querytree);
};

fakebindsforcreateasselect::fakebindsforcreateasselect(
						sqlrtranslations *sqlts,
						xmldomnode *parameters) :
					sqlrtranslation(sqlts,parameters) {
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
stdoutput.printf("faking input binds!!!\n");
		sqlrcur->setFakeInputBindsForThisQuery(true);
	}
	return true;
}

extern "C" {
	sqlrtranslation	*new_fakebindsforcreateasselect(
					sqlrtranslations *sqlts,
					xmldomnode *parameters) {
		return new fakebindsforcreateasselect(sqlts,parameters);
	}
}
