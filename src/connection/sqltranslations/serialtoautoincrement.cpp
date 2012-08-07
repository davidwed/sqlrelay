// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltranslations/serialtoautoincrement.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>

extern "C" {
	sqltranslation	*new_serialtoautoincrement(sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new serialtoautoincrement(sqlts,parameters);
	}
}

serialtoautoincrement::serialtoautoincrement(sqltranslations *sqlts,
						xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool serialtoautoincrement::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();

	// for each column of a create query
	for (xmldomnode *columnnode=querytree->getRootNode()->
				getFirstTagChild(sqlparser::_create)->
				getFirstTagChild(sqlparser::_table)->
				getFirstTagChild(sqlparser::_columns)->
				getFirstTagChild(sqlparser::_column);
		!columnnode->isNullNode();
		columnnode=columnnode->getNextTagSibling(sqlparser::_column)) {

		// get the type node
		xmldomnode	*typenode=
			columnnode->getFirstTagChild(sqlparser::_type);
		if (typenode->isNullNode()) {
			continue;
		}

		// skip non-serial types
		if (charstring::compare(
			typenode->getAttributeValue(sqlparser::_value),
			"serial")) {
			continue;
		}

		// replace serial with int
		typenode->setAttributeValue(sqlparser::_value,"int");

		// find the constraints node or create one
		xmldomnode	*constraintsnode=
			columnnode->getFirstTagChild(sqlparser::_constraints);
		if (constraintsnode->isNullNode()) {
			constraintsnode=sqlts->newNode(columnnode,
						sqlparser::_constraints);
		}

		// add auto_increment constraint
		sqlts->newNode(constraintsnode,sqlparser::_auto_increment);
	}

	return true;
}
