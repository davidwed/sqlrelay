// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltranslations/oracletemptablespreserverowsbydefault.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	sqltranslation	*new_oracletemptablespreserverowsbydefault(
					sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new oracletemptablespreserverowsbydefault(
							sqlts,parameters);
	}
}

oracletemptablespreserverowsbydefault::oracletemptablespreserverowsbydefault(
					sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool oracletemptablespreserverowsbydefault::run(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldom *querytree) {
	debugFunction();

	xmldomnode	*query=querytree->getRootNode();

	// ignore non create-table queries
	xmldomnode	*table=
			query->getFirstTagChild(sqlparser::_create)->
				getFirstTagChild(sqlparser::_table);
	if (table->isNullNode()) {
		return true;
	}

	// ignore non-temporary tables
	xmldomnode	*temporary=
			query->getFirstTagChild(sqlparser::_create)->
				getFirstTagChild(sqlparser::_temporary);
	if (temporary->isNullNode()) {
		return true;
	}

	// if there's already an on commit clause then leave it alone
	xmldomnode	*oncommit=
			table->getFirstTagChild(sqlparser::_on_commit);
	if (!oncommit->isNullNode()) {
		return true;
	}

	// If no on commit clause has been declared then the app expects
	// the default behavior to be followed.  Override that by adding
	// an on commit clause which preserve rows...

	// find the columns clause
	xmldomnode	*columns=table->getFirstTagChild(sqlparser::_columns);

	// if there is no columns clause then bail, we've got bigger problems
	if (columns->isNullNode()) {
		return true;
	}
		
	// add a new on commit clause after the columns clause
	oncommit=sqlts->newNodeAfter(table,columns,sqlparser::_on_commit);
	sqlts->setAttribute(oncommit,sqlparser::_value,"preserve rows");
	return true;
}
