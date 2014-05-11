// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class cascadetocascadeconstraints : public sqlrtranslation {
	public:
			cascadetocascadeconstraints(
						sqlrtranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		void	insertConstraints(xmldomnode *node);
};

cascadetocascadeconstraints::cascadetocascadeconstraints(
						sqlrtranslations *sqlts,
						xmldomnode *parameters) :
					sqlrtranslation(sqlts,parameters) {
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
	sqlrtranslation	*new_cascadetocascadeconstraints(
					sqlrtranslations *sqlts,
					xmldomnode *parameters) {
		return new cascadetocascadeconstraints(sqlts,parameters);
	}
}
