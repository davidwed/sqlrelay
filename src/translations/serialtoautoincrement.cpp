// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlreparser.h>
#include <sqlrelay/sqlrserver.h>
#include <debugprint.h>

class serialtoautoincrement : public sqlrtranslation {
	public:
			serialtoautoincrement(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
};

serialtoautoincrement::serialtoautoincrement(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool serialtoautoincrement::usesTree() {
	return true;
}

bool serialtoautoincrement::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	debugFunction();

	// for each column of a create query
	for (xmldomnode *columnnode=querytree->getRootNode()->
				getFirstTagChild(sqlreparser::_create)->
				getFirstTagChild(sqlreparser::_table)->
				getFirstTagChild(sqlreparser::_columns)->
				getFirstTagChild(sqlreparser::_column);
		!columnnode->isNullNode();
		columnnode=columnnode->getNextTagSibling(sqlreparser::_column)) {

		// get the type node
		xmldomnode	*typenode=
			columnnode->getFirstTagChild(sqlreparser::_type);
		if (typenode->isNullNode()) {
			continue;
		}

		// skip non-serial types
		const char	*coltype=
			typenode->getAttributeValue(sqlreparser::_value);
		bool	serial=!charstring::compare(coltype,"serial");
		bool	serial8=!charstring::compare(coltype,"serial8");
		if (!serial && !serial8) {
			continue;
		}

		// replace column type
		const char	*newcoltype="int";
		if (serial8) {
			// FIXME: for some db's this needs to be a
			// type other than "int" such as long, quad or
			// something else
		}
		typenode->setAttributeValue(sqlreparser::_value,newcoltype);

		// find the constraints node or create one
		xmldomnode	*constraintsnode=
			columnnode->getFirstTagChild(sqlreparser::_constraints);
		if (constraintsnode->isNullNode()) {
			constraintsnode=sqlts->newNode(columnnode,
						sqlreparser::_constraints);
		}

		// add auto_increment constraint
		sqlts->newNode(constraintsnode,sqlreparser::_auto_increment);
	}

	return true;
}

extern "C" {
	sqlrtranslation	*new_serialtoautoincrement(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new serialtoautoincrement(sqlts,parameters,debug);
	}
}
