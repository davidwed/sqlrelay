// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrservercontroller.h>
#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtrigger.h>
#include <sqlrelay/sqlrtranslations.h>
#include <debugprint.h>

class droplocalizedtemptables : public sqlrtrigger {
	public:
			droplocalizedtemptables(
					xmldomnode *parameters, bool debug);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success);
	private:
		bool	dropTable(sqlrservercontroller *cont, xmldom *querytree);
		bool	dropIndex(sqlrservercontroller *cont, xmldom *querytree);
};

droplocalizedtemptables::droplocalizedtemptables(
					xmldomnode *parameters, bool debug) :
					sqlrtrigger(parameters,debug) {
}

bool droplocalizedtemptables::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success) {
	debugFunction();

	if (!querytree) {
		return false;
	}

	// this trigger must be run after a query succeeds
	if (before) {
		return false;
	}
	if (!success) {
		return false;
	}

	return dropTable(sqlrcon->cont,querytree) ||
			dropIndex(sqlrcon->cont,querytree);
}

bool droplocalizedtemptables::dropTable(sqlrservercontroller *cont,
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
		cont->removeReplacementTable(database,schema,table);
	}
	return true;
}

bool droplocalizedtemptables::dropIndex(sqlrservercontroller *cont,
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
	cont->removeReplacementIndex(database,schema,index);
	return true;
}

extern "C" {
	sqlrtrigger	*new_droplocalizedtemptables(
				xmldomnode *parameters, bool debug) {
		return new droplocalizedtemptables(parameters,debug);
	}
}
