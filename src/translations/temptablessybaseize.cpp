// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrcontroller.h>
#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>
#include <rudiments/process.h>

class temptablessybaseize : public sqlrtranslation {
	public:
			temptablessybaseize(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		void		mapCreateTemporaryTableName(
						sqlrconnection_svr *sqlrcon,
						xmldomnode *query);
		void		mapSelectIntoTableName(
						sqlrconnection_svr *sqlrcon,
						xmldomnode *query);
		const char	*generateTempTableName(const char *oldtable);
		bool		replaceTempNames(xmldomnode *node);
};

temptablessybaseize::temptablessybaseize(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool temptablessybaseize::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();

	// for "create temporary table" queries, find the table name,
	// come up with a session-local name for it and put it in the map...
	mapCreateTemporaryTableName(sqlrcon,querytree->getRootNode());

	// for "select ... into table ..." queries, find the table name,
	// come up with a session-local name for it and put it in the map...
	mapSelectIntoTableName(sqlrcon,querytree->getRootNode());

	// for all queries, look for table name nodes and apply the mapping
	return replaceTempNames(querytree->getRootNode());
}

void temptablessybaseize::mapCreateTemporaryTableName(
						sqlrconnection_svr *sqlrcon,
						xmldomnode *node) {
	debugFunction();

	// create...
	xmldomnode	*createnode=
			node->getFirstTagChild(sqlparser::_create);
	if (createnode->isNullNode()) {
		return;
	}

	// global...
	// (might not exist if we're translating queries
	// originally meant for a non-oracle db)
	xmldomnode	*globalnode=
			createnode->getFirstTagChild(sqlparser::_global);

	// temporary...
	xmldomnode	*temporarynode=
			createnode->getFirstTagChild(sqlparser::_temporary);
	if (temporarynode->isNullNode()) {
		return;
	}

	// table...
	xmldomnode	*tablenode=
			temporarynode->getNextTagSibling(sqlparser::_table);
	if (tablenode->isNullNode()) {
		return;
	}

	// table database...
	node=tablenode->getFirstTagChild(sqlparser::_table_name_database);
	const char	*database=node->getAttributeValue(sqlparser::_value);

	// table schema...
	node=tablenode->getFirstTagChild(sqlparser::_table_name_schema);
	const char	*schema=node->getAttributeValue(sqlparser::_value);

	// table name...
	node=tablenode->getFirstTagChild(sqlparser::_table_name_table);
	if (node->isNullNode()) {
		return;
	}
	const char	*oldtable=node->getAttributeValue(sqlparser::_value);

	// create a sybase-ized name and put it in the map...
	databaseobject	*oldtabledbo=sqlts->createDatabaseObject(
						sqlts->temptablepool,
						database,schema,oldtable,NULL);
	const char	*newtable=generateTempTableName(oldtable);
	sqlts->temptablemap.setValue(oldtabledbo,(char *)newtable);

	// remove the global qualifier, if it was found
	if (globalnode) {
		createnode->deleteChild(globalnode);
	}

	// remove the temporary qualifier
	createnode->deleteChild(temporarynode);

	// truncate on commit qualifiers
	xmldomnode	*oncommitnode=
			tablenode->getFirstTagChild(sqlparser::_on_commit);
	if (!oncommitnode->isNullNode()) {
		tablenode->deleteChild(oncommitnode);
	}

	// add table name to drop-at-session-end list
	// (skip the leading #, it will be added by the
	// sqlrcontroller during the drop)
	sqlrcon->cont->addSessionTempTableForDrop(newtable+1);
}

void temptablessybaseize::mapSelectIntoTableName(sqlrconnection_svr *sqlrcon,
						xmldomnode *node) {
	debugFunction();

	// select query
	xmldomnode	*selectnode=
			node->getFirstTagChild(sqlparser::_select);
	if (selectnode->isNullNode()) {
		return;
	}

	// select into
	xmldomnode	*selectintonode=
			selectnode->getFirstTagChild(sqlparser::_select_into);
	if (selectintonode->isNullNode()) {
		return;
	}

	// temporary...
	// (might not exist if we're using sybase/mssql)
	xmldomnode	*temporarynode=
			selectintonode->getFirstTagChild(sqlparser::_temporary);

	// table database...
	node=selectintonode->getFirstTagChild(sqlparser::_table_name_database);
	const char	*database=node->getAttributeValue(sqlparser::_value);

	// table schema...
	node=selectintonode->getFirstTagChild(sqlparser::_table_name_schema);
	const char	*schema=node->getAttributeValue(sqlparser::_value);

	// table name...
	node=selectintonode->getFirstTagChild(sqlparser::_table_name_table);
	if (node->isNullNode()) {
		return;
	}
	const char	*oldtable=node->getAttributeValue(sqlparser::_value);

	// Bail if this is not a temp table.  Select into can be used with
	// regular tables too.
	//
	// Only postgresql and sybase/mssqlserver support "select into table"'s.
	// Postgresql qualifies the table name with temp or temporary and
	// sybase/mssqlserver temp table names start with #'s.
	if (temporarynode->isNullNode() && oldtable[0]!='#') {
		return;
	}

	// create a sybase-ized name and put it in the map...
	databaseobject	*oldtabledbo=sqlts->createDatabaseObject(
						sqlts->temptablepool,
						database,schema,oldtable,NULL);
	const char	*newtable=generateTempTableName(oldtable);
	sqlts->temptablemap.setValue(oldtabledbo,(char *)newtable);

	// remove the temporary qualifier
	if (!temporarynode->isNullNode()) {
		selectintonode->deleteChild(temporarynode);
	}

	// add table name to drop-at-session-end list
	// (skip the leading #, it will be added by the
	// sqlrcontroller during the drop)
	sqlrcon->cont->addSessionTempTableForDrop(newtable+1);
}

const char *temptablessybaseize::generateTempTableName(const char *oldtable) {
	debugFunction();

	// calculate the size we need to store the new table name
	uint64_t	size=1+charstring::length(oldtable)+1;

	// allocate storage for the new table name
	char	*newtable=(char *)sqlts->temptablepool->allocate(size);

	// build up the new table name
	charstring::copy(newtable,"#");
	charstring::append(newtable,oldtable);
	return newtable;
}

bool temptablessybaseize::replaceTempNames(xmldomnode *node) {
	debugFunction();

	// if the current node is a table name
	// then see if it needs to be replaced
	bool	tablenametable=!charstring::compare(node->getName(),
						sqlparser::_table_name_table);
	bool	columnnametable=!charstring::compare(node->getName(),
						sqlparser::_column_name_table);

	if (tablenametable || columnnametable) {

		// get the table name
		const char	*table=node->getAttributeValue(
						sqlparser::_value);

		// try to get the database name
		xmldomnode	*databasenode=
				(tablenametable)?
				node->getPreviousTagSibling(
					sqlparser::_table_name_database):
				node->getPreviousTagSibling(
					sqlparser::_column_name_database);
		const char	*database=databasenode->getAttributeValue(
							sqlparser::_value);

		// try to get the schema name
		xmldomnode	*schemanode=
				(tablenametable)?
				node->getPreviousTagSibling(
					sqlparser::_table_name_schema):
				node->getPreviousTagSibling(
					sqlparser::_column_name_schema);
		const char	*schema=schemanode->getAttributeValue(
							sqlparser::_value);

		// get the replacement table name and update it
		const char	*newname=NULL;
		if (sqlts->getReplacementTableName(database,schema,
							table,&newname)) {
			node->setAttributeValue(sqlparser::_value,newname);
		}
	}

	// if the current node is an index name
	// then see if it needs to be replaced
	if (!charstring::compare(node->getName(),
					sqlparser::_index_name_index)) {

		// get the index name
		const char	*index=node->getAttributeValue(
						sqlparser::_value);

		// try to get the database name
		xmldomnode	*databasenode=
				node->getPreviousTagSibling(
					sqlparser::_index_name_database);
		const char	*database=databasenode->getAttributeValue(
							sqlparser::_value);

		// try to get the schema name
		xmldomnode	*schemanode=
				node->getPreviousTagSibling(
					sqlparser::_index_name_schema);
		const char	*schema=schemanode->getAttributeValue(
							sqlparser::_value);

		// get the replacement index name and update it
		const char	*newname=NULL;
		if (sqlts->getReplacementIndexName(database,schema,
							index,&newname)) {
			node->setAttributeValue(sqlparser::_value,newname);
		}
	}

	// run through child nodes too...
	for (node=node->getFirstTagChild();
			!node->isNullNode();
			node=node->getNextTagSibling()) {
		if (!replaceTempNames(node)) {
			return false;
		}
	}
	return true;
}

extern "C" {
	sqlrtranslation	*new_temptablessybaseize(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new temptablessybaseize(sqlts,parameters,debug);
	}
}
