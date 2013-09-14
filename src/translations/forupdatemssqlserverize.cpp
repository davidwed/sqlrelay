// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslation.h>
#include <debugprint.h>

class forupdatemssqlserverize : public sqltranslation {
	public:
			forupdatemssqlserverize(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldom *querytree);
};

forupdatemssqlserverize::forupdatemssqlserverize(
					sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool forupdatemssqlserverize::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {

	xmldomnode	*query=querytree->getRootNode();

	// select query...
	xmldomnode	*selectnode=query->getFirstTagChild(
						sqlparser::_select);
	if (selectnode->isNullNode()) {
		return true;
	}

	// from...
	xmldomnode	*fromnode=selectnode->getFirstTagChild(
						sqlparser::_from);
	if (fromnode->isNullNode()) {
		return true;
	}

	// table references...
	xmldomnode	*tablereferencesnode=fromnode->getFirstTagChild(
						sqlparser::_table_references);
	if (tablereferencesnode->isNullNode()) {
		return true;
	}

	// for update...
	xmldomnode	*forupdatenode=selectnode->getFirstTagChild(
						sqlparser::_for_update);
	if (forupdatenode->isNullNode()) {
		return true;
	}

	// remove the for update node
	selectnode->deleteChild(forupdatenode);

	// for each table, add a "with (updlock, rowlock)" hint
	for (xmldomnode	*trnode=
			tablereferencesnode->getFirstTagChild(
						sqlparser::_table_reference);
			!trnode->isNullNode();
			trnode=trnode->getNextTagSibling()) {

		// FIXME: do this properly
		sqlts->newNode(trnode,sqlparser::_alias,
				"with (updlock, rowlock)");
	}

	return true;
}

extern "C" {
	sqltranslation	*new_forupdatemssqlserverize(
					sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new forupdatemssqlserverize(sqlts,parameters);
	}
}
