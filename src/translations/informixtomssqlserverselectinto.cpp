// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslation.h>
#include <debugprint.h>

class informixtomssqlserverselectinto : public sqltranslation {
	public:
			informixtomssqlserverselectinto(
						sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
};

informixtomssqlserverselectinto::informixtomssqlserverselectinto(
					sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool informixtomssqlserverselectinto::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();

	xmldomnode	*query=querytree->getRootNode();

	// select query
	xmldomnode	*selectnode=
			query->getFirstTagChild(sqlparser::_select);
	if (selectnode->isNullNode()) {
		return true;
	}

	// select into
	xmldomnode	*selectintonode=
			selectnode->getFirstTagChild(sqlparser::_select_into);
	if (selectintonode->isNullNode()) {
		return true;
	}

	// select expressions
	xmldomnode	*selectexpressionsnode=
			selectnode->getFirstTagChild(
					sqlparser::_select_expressions);
	if (selectexpressionsnode->isNullNode()) {
		return true;
	}

	// move the select into node up to right after the select expressions
	if (!selectnode->moveChild(selectintonode,selectnode,
				selectexpressionsnode->getPosition()+1)) {
	}

	// remove any temporary qualifier
	xmldomnode	*tempnode=
			selectintonode->getFirstTagChild(sqlparser::_temporary);
	if (!tempnode->isNullNode()) {
		selectintonode->deleteChild(tempnode);
	}
	return true;
}

extern "C" {
	sqltranslation	*new_informixtomssqlserverselectinto(
					sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new informixtomssqlserverselectinto(sqlts,parameters);
	}
}
