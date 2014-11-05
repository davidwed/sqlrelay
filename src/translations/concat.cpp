// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrcontroller.h>
#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class concat : public sqlrtranslation {
	public:
			concat(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		bool translateConcat(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *querynode,
					bool *found);
		bool translatePlus(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *querynode,
					bool *found);
		xmldomnode	*parameters;
};

concat::concat(sqlrtranslations *sqlts, xmldomnode *parameters, bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
	this->parameters=parameters;
}

bool concat::run(sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				xmldom *querytree) {

	if (!charstring::compareIgnoringCase(
			parameters->getAttributeValue("operator"),"concat")) {

		// The methods called inside of here rebuild the tree
		// torrentially and it's easy for a "current node" pointer to
		// get lost entirely, so we have to re-search the tree after
		// each translation.  For now at least.
		bool	found=true;
		while (found) {
			if (!translateConcat(sqlrcon,sqlrcur,
					querytree->getRootNode(),&found)) {
				return false;
			}
		}

	} else {

		// The methods called inside of here rebuild the tree
		// torrentially and it's easy for a "current node" pointer to
		// get lost entirely, so we have to re-search the tree after
		// each translation.  For now at least.
		bool	found=true;
		while (found) {
			if (!translatePlus(sqlrcon,sqlrcur,
					querytree->getRootNode(),&found)) {
				return false;
			}
		}
	}
	return true;
}

bool concat::translateConcat(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *querynode,
					bool *found) {
	debugFunction();

	// initialize found flag
	*found=false;

	// look for an expression
	if (!charstring::compare(querynode->getName(),sqlparser::_expression)) {

		// work backwards through the expression, replacing 
		// pipe-concatenated terms with concat()...

		// loop through the children...
		int64_t	i=querynode->getChildCount()-1;
		while (i>=1) {

			// get the child
			xmldomnode	*childnode=querynode->getChild(i);

			// convert || (logical or) to plus
			if (!charstring::compare(childnode->getName(),
						sqlparser::_logical_or)) {
				childnode->setName(sqlparser::_plus);
			}

			// move back through the children
			i--;
		}
	}

	// if we found and converted a || then return
	if (*found) {
		return true;
	}

	// otherwise attempt to convert child nodes...
	for (xmldomnode *node=querynode->getFirstTagChild();
			!node->isNullNode(); node=node->getNextTagSibling()) {
		if (!translateConcat(sqlrcon,sqlrcur,node,found)) {
			return false;
		}
	}
	return true;
}

bool concat::translatePlus(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *querynode,
					bool *found) {
	debugFunction();

	// initialize found flag
	*found=false;

	// look for an expression
	if (!charstring::compare(querynode->getName(),sqlparser::_expression)) {

		// work backwards through the expression, replacing 
		// pipe-concatenated terms with concat()...

		// loop through the children...
		int64_t	i=querynode->getChildCount()-1;
		while (i>=1) {

			// get the child
			xmldomnode	*childnode=querynode->getChild(i);

			// if it's a || (logical or) then convert to concat
			if (!charstring::compare(childnode->getName(),
						sqlparser::_logical_or)) {

				// get the args that were being concatenated
				xmldomnode	*firstarg=
						querynode->getChild(i-1);
				xmldomnode	*secondarg=
						querynode->getChild(i+1);

				// add a concat function
				xmldomnode	*concat=
						sqlts->newNodeAfter(
							querynode,secondarg,
							sqlparser::_function,
							"concat");

				// add parameters to the function
				xmldomnode	*parameters=
						sqlts->newNode(
							concat,
							sqlparser::_parameters);

				// move the first arg to the first parameter
				xmldomnode	*parameter=
						sqlts->newNode(
							parameters,
							sqlparser::_parameter);
				querynode->moveChild(firstarg,parameter,0);

				// move the second arg to the second parameter
				parameter=sqlts->newNode(parameters,
							sqlparser::_parameter);
				querynode->moveChild(secondarg,parameter,0);

				// remove the logical_or
				querynode->deleteChild(childnode);

				// we found one
				*found=true;

				// move back through the children
				// (we really need to move back twice but the
				// i-- below will handle the second move)
				i--;
			}

			// move back through the children
			i--;
		}
	}

	// if we found and converted a || then return
	if (*found) {
		return true;
	}

	// otherwise attempt to convert child nodes...
	for (xmldomnode *node=querynode->getFirstTagChild();
			!node->isNullNode(); node=node->getNextTagSibling()) {
		if (!translateConcat(sqlrcon,sqlrcur,node,found)) {
			return false;
		}
	}
	return true;
}

extern "C" {
	sqlrtranslation	*new_concat(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new concat(sqlts,parameters,debug);
	}
}
