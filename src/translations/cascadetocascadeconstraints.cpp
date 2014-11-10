// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlreparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class cascadetocascadeconstraints : public sqlrtranslation {
	public:
			cascadetocascadeconstraints(
						sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
	private:
		void	insertConstraints(xmldomnode *node);
};

cascadetocascadeconstraints::cascadetocascadeconstraints(
						sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool cascadetocascadeconstraints::usesTree() {
	return true;
}

bool cascadetocascadeconstraints::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	insertConstraints(querytree->getRootNode());
	return true;
}

void cascadetocascadeconstraints::insertConstraints(xmldomnode *node) {

	xmldomnode	*cascade=node->getFirstTagChild(sqlreparser::_drop)->
					getFirstTagChild(sqlreparser::_table)->
					getFirstTagChild(sqlreparser::_cascade);
	if (cascade->isNullNode()) {
		return;
	}

	if (cascade->getFirstTagChild(sqlreparser::_cascade_constraints_clause)->
								isNullNode()) {
		cascade->appendTag(sqlreparser::_cascade_constraints_clause);
	}
}

extern "C" {
	sqlrtranslation	*new_cascadetocascadeconstraints(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new cascadetocascadeconstraints(sqlts,parameters,debug);
	}
}
