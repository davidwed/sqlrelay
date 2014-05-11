// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslation.h>
#include <debugprint.h>

class cascadetocascadeconstraints : public sqltranslation {
	public:
			cascadetocascadeconstraints(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		void	insertConstraints(xmldomnode *node);
};

cascadetocascadeconstraints::cascadetocascadeconstraints(sqltranslations *sqlts,
						xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool cascadetocascadeconstraints::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	insertConstraints(querytree->getRootNode());
	return true;
}

void cascadetocascadeconstraints::insertConstraints(xmldomnode *node) {

	xmldomnode	*cascade=node->getFirstTagChild(sqlparser::_drop)->
					getFirstTagChild(sqlparser::_table)->
					getFirstTagChild(sqlparser::_cascade);
	if (cascade->isNullNode()) {
		return;
	}

	if (cascade->getFirstTagChild(sqlparser::_cascade_constraints_clause)->
								isNullNode()) {
		cascade->appendTag(sqlparser::_cascade_constraints_clause);
	}
}

extern "C" {
	sqltranslation	*new_cascadetocascadeconstraints(sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new cascadetocascadeconstraints(sqlts,parameters);
	}
}
