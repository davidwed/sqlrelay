// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <sqlrelay/sqlreparser.h>
#include <debugprint.h>

class matchestolike : public sqlrtranslation {
	public:
			matchestolike(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
	private:
		bool	replaceMatchesWithLike(xmldomnode *node);
		void	wrap(xmldomnode *node);
};

matchestolike::matchestolike(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool matchestolike::usesTree() {
	return true;
}

bool matchestolike::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	return replaceMatchesWithLike(querytree->getRootNode());
}

bool matchestolike::replaceMatchesWithLike(xmldomnode *node) {

	if (node->isNullNode()) {
		return true;
	}

	// if this is a "matches" node
	if (!charstring::compare(node->getName(),sqlreparser::_matches)) {

		// change it to a "like" node
		node->setName(sqlreparser::_like);

		// wrap the following expression with %'s
		xmldomnode	*expression=node->getNextTagSibling();
		if (!charstring::compare(expression->getName(),
					sqlreparser::_expression)) {
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
						sqlreparser::_function,
						"replace");
	xmldomnode	*replace1params=sqlts->newNode(replace1func,
							sqlreparser::_parameters);
	xmldomnode	*replace1parameter1=sqlts->newNode(replace1params,
							sqlreparser::_parameter);
	xmldomnode	*replace1expression1=sqlts->newNode(replace1parameter1,
							sqlreparser::_expression);
	node->getParent()->moveChild(node,replace1expression1,0);
	xmldomnode	*replace1parameter2=sqlts->newNode(replace1params,
							sqlreparser::_parameter);
	xmldomnode	*replace1expression2=sqlts->newNode(replace1parameter2,
							sqlreparser::_expression);
	sqlts->newNode(replace1expression2,sqlreparser::_string_literal,"'*'");
	xmldomnode	*replace1parameter3=sqlts->newNode(replace1params,
							sqlreparser::_parameter);
	xmldomnode	*replace1expression3=sqlts->newNode(replace1parameter3,
							sqlreparser::_expression);
	sqlts->newNode(replace1expression3,sqlreparser::_string_literal,"'%'");

	// wrap -> replace(replace(...),'_','?')
	xmldomnode	*replace2func=sqlts->newNodeAfter(
						replace1func->getParent(),
						replace1func,
						sqlreparser::_function,
						"replace");
	xmldomnode	*replace2params=sqlts->newNode(replace2func,
							sqlreparser::_parameters);
	xmldomnode	*replace2parameter1=sqlts->newNode(replace2params,
							sqlreparser::_parameter);
	xmldomnode	*replace2expression1=sqlts->newNode(replace2parameter1,
							sqlreparser::_expression);
	replace1func->getParent()->moveChild(replace1func,
						replace2expression1,0);
	xmldomnode	*replace2parameter2=sqlts->newNode(replace2params,
							sqlreparser::_parameter);
	xmldomnode	*replace2expression2=sqlts->newNode(replace2parameter2,
							sqlreparser::_expression);
	sqlts->newNode(replace2expression2,sqlreparser::_string_literal,"'?'");
	xmldomnode	*replace2parameter3=sqlts->newNode(replace2params,
							sqlreparser::_parameter);
	xmldomnode	*replace2expression3=sqlts->newNode(replace2parameter3,
							sqlreparser::_expression);
	sqlts->newNode(replace2expression3,sqlreparser::_string_literal,"'_'");

	// FIXME: what about []'s and [^]'s?
}

extern "C" {
	sqlrtranslation	*new_matchestolike(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) {
		return new matchestolike(sqlts,parameters,debug);
	}
}
