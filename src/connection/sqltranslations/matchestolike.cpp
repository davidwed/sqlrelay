// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltranslations/matchestolike.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>

extern "C" {
	sqltranslation	*new_matchestolike(sqltranslations *sqlts,
						xmldomnode *parameters) {
		return new matchestolike(sqlts,parameters);
	}
}

matchestolike::matchestolike(sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool matchestolike::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	return replaceMatchesWithLike(querytree->getRootNode());
}

bool matchestolike::replaceMatchesWithLike(xmldomnode *node) {

	if (node->isNullNode()) {
		return true;
	}

	// if this is a "matches" node
	if (!charstring::compare(node->getName(),sqlparser::_matches)) {

		// change it to a "like" node
		node->setName(sqlparser::_like);

		// wrap the following expression with %'s
		xmldomnode	*expression=node->getNextTagSibling();
		if (!charstring::compare(expression->getName(),
					sqlparser::_expression)) {
			wrapConcat(expression);
		}
	}

	// run on children, then next sibling
	return replaceMatchesWithLike(node->getFirstTagChild()) &&
		replaceMatchesWithLike(node->getNextTagSibling());
}

void matchestolike::wrapConcat(xmldomnode *node) {

	// wrap -> concat(...expression...,'%')
	xmldomnode	*concat1func=sqlts->newNode(node->getParent(),
							sqlparser::_function,
							"concat");
	xmldomnode	*concat1params=sqlts->newNode(concat1func,
							sqlparser::_parameters);
	xmldomnode	*concat1parameter1=sqlts->newNode(concat1params,
							sqlparser::_parameter);
	xmldomnode	*concat1expression1=sqlts->newNode(concat1parameter1,
							sqlparser::_expression);
	node->getParent()->moveChild(node,concat1expression1,0);
	xmldomnode	*concat1parameter2=sqlts->newNode(concat1params,
							sqlparser::_parameter);
	xmldomnode	*concat1expression2=sqlts->newNode(concat1parameter2,
							sqlparser::_expression);
	sqlts->newNode(concat1expression2,sqlparser::_string_literal,"'%'");
	
	// wrap -> concat('%',concat(...expression...,'%'))
	xmldomnode	*concat2func=sqlts->newNode(concat1func->getParent(),
							sqlparser::_function,
							"concat");
	xmldomnode	*concat2params=sqlts->newNode(concat2func,
							sqlparser::_parameters);
	xmldomnode	*concat2parameter1=sqlts->newNode(concat2params,
							sqlparser::_parameter);
	xmldomnode	*concat2expression1=sqlts->newNode(concat2parameter1,
							sqlparser::_expression);
	sqlts->newNode(concat2expression1,sqlparser::_string_literal,"'%'");
	xmldomnode	*concat2parameter2=sqlts->newNode(concat2params,
							sqlparser::_parameter);
	xmldomnode	*concat2expression2=sqlts->newNode(concat2parameter2,
							sqlparser::_expression);
	concat1func->getParent()->moveChild(concat1func,concat2expression2,0);
}
