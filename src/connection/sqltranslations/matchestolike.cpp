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
			wrap(expression);
		}
	}

	// run on children, then next sibling
	return replaceMatchesWithLike(node->getFirstTagChild()) &&
		replaceMatchesWithLike(node->getNextTagSibling());
}

void matchestolike::wrap(xmldomnode *node) {

	// wrap -> concat(...expression...,'%')
	/*xmldomnode	*concat1func=sqlts->newNodeAfter(node->getParent(),
							node,
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
	xmldomnode	*concat2func=sqlts->newNodeAfter(
						concat1func->getParent(),
						concat1func,
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
	concat1func->getParent()->moveChild(concat1func,concat2expression2,0);*/

	// wrap -> replace(...,'*','%')
	xmldomnode	*replace1func=sqlts->newNodeAfter(
						node->getParent(),
						node,
						sqlparser::_function,
						"replace");
	xmldomnode	*replace1params=sqlts->newNode(replace1func,
							sqlparser::_parameters);
	xmldomnode	*replace1parameter1=sqlts->newNode(replace1params,
							sqlparser::_parameter);
	xmldomnode	*replace1expression1=sqlts->newNode(replace1parameter1,
							sqlparser::_expression);
	node->getParent()->moveChild(node,replace1expression1,0);
	xmldomnode	*replace1parameter2=sqlts->newNode(replace1params,
							sqlparser::_parameter);
	xmldomnode	*replace1expression2=sqlts->newNode(replace1parameter2,
							sqlparser::_expression);
	sqlts->newNode(replace1expression2,sqlparser::_string_literal,"'*'");
	xmldomnode	*replace1parameter3=sqlts->newNode(replace1params,
							sqlparser::_parameter);
	xmldomnode	*replace1expression3=sqlts->newNode(replace1parameter3,
							sqlparser::_expression);
	sqlts->newNode(replace1expression3,sqlparser::_string_literal,"'%'");

	// wrap -> replace(replace(...),'_','?')
	xmldomnode	*replace2func=sqlts->newNodeAfter(
						replace1func->getParent(),
						replace1func,
						sqlparser::_function,
						"replace");
	xmldomnode	*replace2params=sqlts->newNode(replace2func,
							sqlparser::_parameters);
	xmldomnode	*replace2parameter1=sqlts->newNode(replace2params,
							sqlparser::_parameter);
	xmldomnode	*replace2expression1=sqlts->newNode(replace2parameter1,
							sqlparser::_expression);
	replace1func->getParent()->moveChild(replace1func,
						replace2expression1,0);
	xmldomnode	*replace2parameter2=sqlts->newNode(replace2params,
							sqlparser::_parameter);
	xmldomnode	*replace2expression2=sqlts->newNode(replace2parameter2,
							sqlparser::_expression);
	sqlts->newNode(replace2expression2,sqlparser::_string_literal,"'?'");
	xmldomnode	*replace2parameter3=sqlts->newNode(replace2params,
							sqlparser::_parameter);
	xmldomnode	*replace2expression3=sqlts->newNode(replace2parameter3,
							sqlparser::_expression);
	sqlts->newNode(replace2expression3,sqlparser::_string_literal,"'_'");

	// FIXME: what about []'s and [^]'s?
}
