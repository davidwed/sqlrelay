// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrservercontroller.h>
#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlreparser.h>
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
					getFirstTagChild(sqlreparser::_drop);
	if (node->isNullNode()) {
		return false;
	}

	// table...
	node=node->getFirstTagChild(sqlreparser::_table);
	if (node->isNullNode()) {
		return false;
	}

	// table name list...
	node=node->getFirstTagChild(sqlreparser::_table_name_list);
	if (node->isNullNode()) {
		return false;
	}

	// for each table name...
	for (xmldomnode *listitemnode=
			node->getFirstTagChild(
				sqlreparser::_table_name_list_item);
		!listitemnode->isNullNode();
			listitemnode=listitemnode->
				getNextTagSibling(
					sqlreparser::_table_name_list_item)) {

		// table name database and schema...
		node=listitemnode->getFirstTagChild(
					sqlreparser::_table_name_database);
		const char	*database=
				node->getAttributeValue(sqlreparser::_value);
		node=listitemnode->getFirstTagChild(
					sqlreparser::_table_name_schema);
		const char	*schema=
				node->getAttributeValue(sqlreparser::_value);

		// table name...
		node=listitemnode->getFirstTagChild(
					sqlreparser::_table_name_table);
		if (node->isNullNode()) {
			continue;
		}
		const char	*table=
				node->getAttributeValue(sqlreparser::_value);
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
					getFirstTagChild(sqlreparser::_drop);
	if (node->isNullNode()) {
		return false;
	}

	// index...
	xmldomnode	*indexnode=node->getFirstTagChild(sqlreparser::_index);
	if (indexnode->isNullNode()) {
		return false;
	}

	// index name database and schema...
	node=indexnode->getFirstTagChild(sqlreparser::_index_name_database);
	const char	*database=node->getAttributeValue(sqlreparser::_value);
	node=indexnode->getFirstTagChild(sqlreparser::_index_name_schema);
	const char	*schema=node->getAttributeValue(sqlreparser::_value);

	// index name...
	node=indexnode->getFirstTagChild(sqlreparser::_index_name_index);
	if (node->isNullNode()) {
		return false;
	}
	const char	*index=node->getAttributeValue(sqlreparser::_value);
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
