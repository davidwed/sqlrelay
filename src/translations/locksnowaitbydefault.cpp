// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslation.h>
#include <debugprint.h>

class locksnowaitbydefault : public sqltranslation {
	public:
			locksnowaitbydefault(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
};

locksnowaitbydefault::locksnowaitbydefault(sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool locksnowaitbydefault::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {

	xmldomnode	*query=querytree->getRootNode();

	// lock query...
	xmldomnode	*locknode=query->getFirstTagChild(sqlparser::_lock);
	if (!locknode->isNullNode()) {

		// see if there's already a nowait argument
		xmldomnode	*nowaitnode=
				locknode->getFirstTagChild(sqlparser::_nowait);
		if (!nowaitnode->isNullNode()) {
			// if there was one then we don't have anything to do
			return true;
		}

		// add a nowait argument
		sqlts->newNodeAfter(locknode,
				locknode->getChild(locknode->getChildCount()-1),
				sqlparser::_nowait);
		return true;
	}


	// select query...
	xmldomnode	*selectnode=query->getFirstTagChild(sqlparser::_select);
	if (!selectnode->isNullNode()) {

		// see if there's already a nowait argument
		xmldomnode	*nowaitnode=
			selectnode->getFirstTagChild(sqlparser::_nowait);
		if (!nowaitnode->isNullNode()) {
			// if there was one then we don't have anything to do
			return true;
		}

		// find the for update node
		xmldomnode	*forupdatenode=
			selectnode->getFirstTagChild(sqlparser::_for_update);
		if (forupdatenode->isNullNode()) {
			// if there wasn't one then we don't have anything to do
			return true;
		}

		// add a nowait argument
		sqlts->newNodeAfter(selectnode,
				forupdatenode,sqlparser::_nowait);
		return true;
	}

	return true;
}

extern "C" {
	sqltranslation	*new_locksnowaitbydefault(
					sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new locksnowaitbydefault(sqlts,parameters);
	}
}
