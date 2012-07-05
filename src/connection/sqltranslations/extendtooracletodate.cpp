// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltranslations/extendtooracletodate.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>

extendtooracletodate::extendtooracletodate(sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool extendtooracletodate::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {

	return translateExtends(sqlrcon,sqlrcur,querytree->getRootNode());
}

bool extendtooracletodate::translateExtends(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *querynode) {
	debugFunction();

	// look for extend function
	if (!charstring::compare(
			querynode->getName(),sqlparser::_function) &&
		!charstring::compareIgnoringCase(
			querynode->getAttributeValue(sqlparser::_value),
			"extend")) {

		// translate it
		if (!translateExtend(sqlrcon,sqlrcur,querynode)) {
			return false;
		}
	}

	// convert child nodes...
	for (xmldomnode *node=querynode->getFirstTagChild();
			!node->isNullNode(); node=node->getNextTagSibling()) {
		if (!translateExtends(sqlrcon,sqlrcur,node)) {
			return false;
		}
	}
	return true;
}

bool extendtooracletodate::translateExtend(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *querynode) {
	debugFunction();

	// FIXME: implement this...
	return true;
}
