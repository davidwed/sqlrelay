// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltriggers/droplocalizedtemptables.h>
#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	sqltrigger	*new_droplocalizedtemptables(
					xmldomnode *parameters) {
		return new droplocalizedtemptables(parameters);
	}
}

droplocalizedtemptables::droplocalizedtemptables(
			xmldomnode *parameters) : sqltrigger(parameters) {
}

bool droplocalizedtemptables::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success) {
	debugFunction();

	// this trigger must be run after a query succeeds
	if (before) {
		return false;
	}
	if (!success) {
		return false;
	}

	// sqltranslations must exist too
	sqltranslations	*sqlt=sqlrcon->cont->sqlt;
	if (!sqlt) {
		return false;
	}

	return dropTable(sqlt,querytree) || dropIndex(sqlt,querytree);
}

bool droplocalizedtemptables::dropTable(sqltranslations *sqlt,
							xmldom *querytree) {
	debugFunction();

	// drop...
	xmldomnode	*node=querytree->getRootNode()->
					getFirstTagChild(sqlparser::_drop);
	if (node->isNullNode()) {
		return false;
	}

	// table...
	node=node->getFirstTagChild(sqlparser::_table);
	if (node->isNullNode()) {
		return false;
	}

	// table name list...
	node=node->getFirstTagChild(sqlparser::_table_name_list);
	if (node->isNullNode()) {
		return false;
	}

	// for each table name...
	for (xmldomnode *listitemnode=
			node->getFirstTagChild(
				sqlparser::_table_name_list_item);
		!listitemnode->isNullNode();
			listitemnode=listitemnode->
				getNextTagSibling(
					sqlparser::_table_name_list_item)) {

		// table name database and schema...
		node=listitemnode->getFirstTagChild(
					sqlparser::_table_name_database);
		const char	*database=
				node->getAttributeValue(sqlparser::_value);
		node=listitemnode->getFirstTagChild(
					sqlparser::_table_name_schema);
		const char	*schema=
				node->getAttributeValue(sqlparser::_value);

		// table name...
		node=listitemnode->getFirstTagChild(
					sqlparser::_table_name_table);
		if (node->isNullNode()) {
			continue;
		}
		const char	*table=
				node->getAttributeValue(sqlparser::_value);
		if (!table) {
			continue;
		}

		// unmap the table (and any indices that depend on it)
		sqlt->removeReplacementTable(database,schema,table);
	}
	return true;
}

bool droplocalizedtemptables::dropIndex(sqltranslations *sqlt,
							xmldom *querytree) {
	debugFunction();

	// drop...
	xmldomnode	*node=querytree->getRootNode()->
					getFirstTagChild(sqlparser::_drop);
	if (node->isNullNode()) {
		return false;
	}

	// index...
	xmldomnode	*indexnode=node->getFirstTagChild(sqlparser::_index);
	if (indexnode->isNullNode()) {
		return false;
	}

	// index name database and schema...
	node=indexnode->getFirstTagChild(sqlparser::_index_name_database);
	const char	*database=node->getAttributeValue(sqlparser::_value);
	node=indexnode->getFirstTagChild(sqlparser::_index_name_schema);
	const char	*schema=node->getAttributeValue(sqlparser::_value);

	// index name...
	node=indexnode->getFirstTagChild(sqlparser::_index_name_index);
	if (node->isNullNode()) {
		return false;
	}
	const char	*index=node->getAttributeValue(sqlparser::_value);
	if (!index) {
		return false;
	}

	// unmap the index
	sqlt->removeReplacementIndex(database,schema,index);
	return true;
}
