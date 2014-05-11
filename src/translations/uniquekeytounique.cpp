// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslation.h>
#include <debugprint.h>

class uniquekeytounique : public sqltranslation {
	public:
			uniquekeytounique(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		void	replaceUniqueKey(xmldomnode *node);
};

uniquekeytounique::uniquekeytounique(sqltranslations *sqlts,
						xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool uniquekeytounique::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	replaceUniqueKey(querytree->getRootNode());
	return true;
}

void uniquekeytounique::replaceUniqueKey(xmldomnode *node) {

	xmldomnode	*columns=node->getFirstTagChild(sqlparser::_create)->
					getFirstTagChild(sqlparser::_table)->
					getFirstTagChild(sqlparser::_columns);
	if (columns->isNullNode()) {
		return;
	}

	for (xmldomnode *col=columns->getFirstTagChild(sqlparser::_column);
			!col->isNullNode();
			col=col->getNextTagSibling(sqlparser::_column)) {

		xmldomnode	*uniquekey=
			col->getFirstTagChild(sqlparser::_constraints)->
				getFirstTagChild(sqlparser::_unique_key);
		if (!uniquekey->isNullNode()) {
			uniquekey->setName(sqlparser::_unique);
		}
	}
}

extern "C" {
	sqltranslation	*new_uniquekeytounique(sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new uniquekeytounique(sqlts,parameters);
	}
}
