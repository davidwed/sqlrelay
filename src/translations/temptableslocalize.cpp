// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrservercontroller.h>
#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>
#include <rudiments/process.h>

class temptableslocalize : public sqlrtranslation {
	public:
			temptableslocalize(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
	private:
		void		mapCreateTemporaryTableName(
						sqlrserverconnection *sqlrcon,
						xmldomnode *query,
						const char *uniqueid);
		void		mapCreateIndexOnTemporaryTableName(
						xmldomnode *query,
						const char *uniqueid);
		void		mapSelectIntoTableName(
						sqlrserverconnection *sqlrcon,
						xmldomnode *query,
						const char *uniqueid);
		const char	*generateTempTableName(const char *oldtable,
							const char *uniqueid);
		bool		replaceTempNames(xmldomnode *node);
};

temptableslocalize::temptableslocalize(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool temptableslocalize::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	debugFunction();

	// get the unique id
	const char	*uniqueid=parameters->getAttributeValue("uniqueid");
	if (!uniqueid) {
		uniqueid="";
	}

	// for "create temporary table" queries, find the table name,
	// come up with a session-local name for it and put it in the map...
	mapCreateTemporaryTableName(sqlrcon,querytree->getRootNode(),uniqueid);

	// for "create index" queries that refer to temporary tables, find the
	// index name, come up with a session-local name for it and put it in
	// the map...
	mapCreateIndexOnTemporaryTableName(querytree->getRootNode(),uniqueid);

	// for "select ... into table ..." queries, find the table name,
	// come up with a session-local name for it and put it in the map...
	mapSelectIntoTableName(sqlrcon,querytree->getRootNode(),uniqueid);

	// for all queries, look for table name nodes and apply the mapping
	return replaceTempNames(querytree->getRootNode());
}

void temptableslocalize::mapCreateTemporaryTableName(
						sqlrserverconnection *sqlrcon,
						xmldomnode *node,
						const char *uniqueid) {
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

	// create a session-local name and put it in the map...
	databaseobject	*oldtabledbo=sqlts->createDatabaseObject(
						sqlts->temptablepool,
						database,schema,oldtable,NULL);
	const char	*newtable=generateTempTableName(oldtable,uniqueid);
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
	sqlrcon->cont->addSessionTempTableForDrop(newtable);
}

void temptableslocalize::mapCreateIndexOnTemporaryTableName(xmldomnode *node,
							const char *uniqueid) {
	debugFunction();

	// create...
	node=node->getFirstTagChild(sqlparser::_create);
	if (node->isNullNode()) {
		return;
	}

	// index...
	xmldomnode	*indexnode=node->getFirstTagChild(sqlparser::_index);
	if (indexnode->isNullNode()) {
		return;
	}

	// index database...
	node=indexnode->getFirstTagChild(sqlparser::_index_name_database);
	const char	*indexdatabase=
			node->getAttributeValue(sqlparser::_value);

	// index schema...
	node=indexnode->getFirstTagChild(sqlparser::_index_name_schema);
	const char	*indexschema=
			node->getAttributeValue(sqlparser::_value);

	// index name...
	node=indexnode->getFirstTagChild(sqlparser::_index_name_index);
	if (node->isNullNode()) {
		return;
	}
	const char	*oldindex=node->getAttributeValue(sqlparser::_value);

	// table database...
	node=indexnode->getFirstTagChild(sqlparser::_table_name_database);
	const char	*tabledatabase=
			node->getAttributeValue(sqlparser::_value);

	// table schema...
	node=indexnode->getFirstTagChild(sqlparser::_table_name_schema);
	const char	*tableschema=
			node->getAttributeValue(sqlparser::_value);

	// table name...
	node=indexnode->getFirstTagChild(sqlparser::_table_name_table);
	if (node->isNullNode()) {
		return;
	}
	const char	*oldtable=node->getAttributeValue(sqlparser::_value);

	// if the table name isn't in the temp table map then ignore this query
	const char	*newtable=NULL;
	if (!sqlts->getReplacementTableName(tabledatabase,tableschema,
						oldtable,&newtable)) {
		return;
	}

	// create a session-local name and put it in the map...
	databaseobject	*oldindexdbo=sqlts->createDatabaseObject(
						sqlts->tempindexpool,
						indexdatabase,
						indexschema,
						oldindex,
						newtable);
	const char	*newindex=generateTempTableName(oldindex,uniqueid);
	sqlts->tempindexmap.setValue(oldindexdbo,(char *)newindex);
}

void temptableslocalize::mapSelectIntoTableName(sqlrserverconnection *sqlrcon,
							xmldomnode *node,
							const char *uniqueid) {
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

	// create a session-local name and put it in the map...
	databaseobject	*oldtabledbo=sqlts->createDatabaseObject(
						sqlts->temptablepool,
						database,schema,oldtable,NULL);
	const char	*newtable=generateTempTableName(oldtable,uniqueid);
	sqlts->temptablemap.setValue(oldtabledbo,(char *)newtable);

	// remove the temporary qualifier
	if (!temporarynode->isNullNode()) {
		selectintonode->deleteChild(temporarynode);
	}

	// add table name to drop-at-session-end list
	sqlrcon->cont->addSessionTempTableForDrop(newtable);
}

const char *temptableslocalize::generateTempTableName(const char *oldtable,
							const char *uniqueid) {
	debugFunction();

	// Sybase and mssqlserver prepend #'s to temp tables.  We need to
	// remove those.
	if (oldtable[0]=='#') {
		oldtable++;
	}

	// get the process id
	uint64_t	pid=process::getProcessId();

	// calculate the size we need to store the new table name
	uint64_t	size=charstring::length(oldtable)+1+
				charstring::length(uniqueid)+1+
				charstring::integerLength(pid)+1;

	// allocate storage for the new table name
	char	*newtable=(char *)sqlts->temptablepool->allocate(size);

	// build up the new table name
	charstring::copy(newtable,oldtable);
	charstring::append(newtable,"_");
	charstring::append(newtable,uniqueid);
	charstring::append(newtable,"_");
	charstring::append(newtable,pid);
	return newtable;
}

bool temptableslocalize::replaceTempNames(xmldomnode *node) {
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
	sqlrtranslation	*new_temptableslocalize(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new temptableslocalize(sqlts,parameters,debug);
	}
}
