// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <sqlrelay/sqlreparser.h>
#include <debugprint.h>

class informixtomssqlserverselectinto : public sqlrtranslation {
	public:
			informixtomssqlserverselectinto(
						sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
};

informixtomssqlserverselectinto::informixtomssqlserverselectinto(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool informixtomssqlserverselectinto::usesTree() {
	return true;
}

bool informixtomssqlserverselectinto::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	debugFunction();

	xmldomnode	*query=querytree->getRootNode();

	// select query
	xmldomnode	*selectnode=
			query->getFirstTagChild(sqlreparser::_select);
	if (selectnode->isNullNode()) {
		return true;
	}

	// select into
	xmldomnode	*selectintonode=
			selectnode->getFirstTagChild(sqlreparser::_select_into);
	if (selectintonode->isNullNode()) {
		return true;
	}

	// select expressions
	xmldomnode	*selectexpressionsnode=
			selectnode->getFirstTagChild(
					sqlreparser::_select_expressions);
	if (selectexpressionsnode->isNullNode()) {
		return true;
	}

	// move the select into node up to right after the select expressions
	if (!selectnode->moveChild(selectintonode,selectnode,
				selectexpressionsnode->getPosition()+1)) {
	}

	// remove any temporary qualifier
	xmldomnode	*tempnode=
			selectintonode->getFirstTagChild(sqlreparser::_temporary);
	if (!tempnode->isNullNode()) {
		selectintonode->deleteChild(tempnode);
	}
	return true;
}

extern "C" {
	sqlrtranslation	*new_informixtomssqlserverselectinto(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new informixtomssqlserverselectinto(
					sqlts,parameters,debug);
	}
}
