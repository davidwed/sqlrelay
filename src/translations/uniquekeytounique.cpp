// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class uniquekeytounique : public sqlrtranslation {
	public:
			uniquekeytounique(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		void	replaceUniqueKey(xmldomnode *node);
};

uniquekeytounique::uniquekeytounique(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
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
	sqlrtranslation	*new_uniquekeytounique(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) {
		return new uniquekeytounique(sqlts,parameters,debug);
	}
}
