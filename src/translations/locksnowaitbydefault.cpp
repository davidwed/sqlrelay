// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class locksnowaitbydefault : public sqlrtranslation {
	public:
			locksnowaitbydefault(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
};

locksnowaitbydefault::locksnowaitbydefault(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool locksnowaitbydefault::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
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
	sqlrtranslation	*new_locksnowaitbydefault(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new locksnowaitbydefault(sqlts,parameters,debug);
	}
}
