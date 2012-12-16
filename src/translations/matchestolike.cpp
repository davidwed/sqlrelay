// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslation.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class matchestolike : public sqltranslation {
	public:
			matchestolike(sqltranslations *sqlts,
					xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		bool	replaceMatchesWithLike(xmldomnode *node);
		void	wrap(xmldomnode *node);
};

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

extern "C" {
	sqltranslation	*new_matchestolike(sqltranslations *sqlts,
						xmldomnode *parameters) {
		return new matchestolike(sqlts,parameters);
	}
}
