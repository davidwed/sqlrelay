// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlreparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class uniquekeytounique : public sqlrtranslation {
	public:
			uniquekeytounique(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
	private:
		void	replaceUniqueKey(xmldomnode *node);
};

uniquekeytounique::uniquekeytounique(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool uniquekeytounique::usesTree() {
	return true;
}

bool uniquekeytounique::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	replaceUniqueKey(querytree->getRootNode());
	return true;
}

void uniquekeytounique::replaceUniqueKey(xmldomnode *node) {

	xmldomnode	*columns=node->getFirstTagChild(sqlreparser::_create)->
					getFirstTagChild(sqlreparser::_table)->
					getFirstTagChild(sqlreparser::_columns);
	if (columns->isNullNode()) {
		return;
	}

	for (xmldomnode *col=columns->getFirstTagChild(sqlreparser::_column);
			!col->isNullNode();
			col=col->getNextTagSibling(sqlreparser::_column)) {

		xmldomnode	*uniquekey=
			col->getFirstTagChild(sqlreparser::_constraints)->
				getFirstTagChild(sqlreparser::_unique_key);
		if (!uniquekey->isNullNode()) {
			uniquekey->setName(sqlreparser::_unique);
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
