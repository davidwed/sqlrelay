// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltranslations/temptableslocalize.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>
#include <rudiments/process.h>

temptableslocalize::temptableslocalize(sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool temptableslocalize::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {

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

	// for all queries, look for table name nodes or verbatim nodes and
	// apply the mapping
	return replaceTempNames(querytree->getRootNode());
}

void temptableslocalize::mapCreateTemporaryTableName(
						sqlrconnection_svr *sqlrcon,
						xmldomnode *node,
						const char *uniqueid) {

	// create...
	node=node->getFirstTagChild(sqlparser::_create);
	if (node->isNullNode()) {
		return;
	}
	xmldomnode	*createnode=node;

	// temporary...
	node=node->getFirstTagChild(sqlparser::_temporary);
	if (node->isNullNode()) {
		return;
	}
	xmldomnode	*temporarynode=node;

	// table...
	node=node->getNextTagSibling(sqlparser::_table);
	if (node->isNullNode()) {
		return;
	}
	xmldomnode	*tablenode=node;

	// table name...
	node=node->getFirstTagChild(sqlparser::_table_name);
	if (node->isNullNode()) {
		return;
	}

	// create a session-local name and put it in the map...
	const char	*oldtablename=
			node->getAttributeValue(sqlparser::_value);
	uint64_t	size=charstring::length(oldtablename)+1;
	char		*oldtablenamecopy=
			(char *)sqlts->temptablepool->malloc(size);
	charstring::copy(oldtablenamecopy,oldtablename);
	const char	*newtablename=
			generateTempTableName(oldtablename,uniqueid);
	sqlts->temptablemap.setData((char *)oldtablenamecopy,
					(char *)newtablename);

	// remove the temporary qualifier
	createnode->deleteChild(temporarynode);

	// truncate on commit qualifiers
	xmldomnode	*oncommitnode=
			tablenode->getFirstTagChild(sqlparser::_on_commit);
	if (!oncommitnode->isNullNode()) {
		tablenode->deleteChild(oncommitnode);
	}

	// add table name to drop-at-session-end list
	sqlrcon->addSessionTempTableForDrop(newtablename);
}

const char *temptableslocalize::generateTempTableName(const char *oldname,
							const char *uniqueid) {

	uint64_t	pid=process::getProcessId();
	uint64_t	size=charstring::length(oldname)+1+
				charstring::length(uniqueid)+1+
				charstring::integerLength(pid)+1;
	char	*newname=(char *)sqlts->temptablepool->malloc(size);
	snprintf(newname,size,"%s_%s_%lld",oldname,uniqueid,pid);
	return newname;
}

void temptableslocalize::mapCreateIndexOnTemporaryTableName(xmldomnode *node,
							const char *uniqueid) {

	// create...
	node=node->getFirstTagChild(sqlparser::_create);
	if (node->isNullNode()) {
		return;
	}

	// index...
	node=node->getFirstTagChild(sqlparser::_index);
	if (node->isNullNode()) {
		return;
	}

	// index name...
	xmldomnode	*indexnamenode=
			node->getFirstTagChild(sqlparser::_index_name);
	if (indexnamenode->isNullNode()) {
		return;
	}

	// table name...
	node=node->getFirstTagChild(sqlparser::_table_name);
	if (node->isNullNode()) {
		return;
	}

	// if the table name isn't in the temp table map then ignore this query
	char	*newname;
	if (!sqlts->temptablemap.getData((char *)node->getAttributeValue(
						sqlparser::_value),&newname)) {
		return;
	}

	// create a session-local name and put it in the map...
	const char	*oldindexname=
			indexnamenode->getAttributeValue(sqlparser::_value);
	uint64_t	size=charstring::length(oldindexname)+1;
	char		*oldindexnamecopy=
			(char *)sqlts->tempindexpool->malloc(size);
	charstring::copy(oldindexnamecopy,oldindexname);
	const char	*newindexname=
			generateTempTableName(oldindexname,uniqueid);
	sqlts->tempindexmap.setData((char *)oldindexnamecopy,
					(char *)newindexname);
}

bool temptableslocalize::replaceTempNames(xmldomnode *node) {

	// if the current node is a table name
	// then see if it needs to be replaced
	if (!charstring::compare(node->getName(),
					sqlparser::_table_name) ||
		!charstring::compare(node->getName(),
					sqlparser::_column_name_table)) {

		const char	*newname=NULL;
		const char	*value=node->getAttributeValue(
						sqlparser::_value);

		if (sqlts->getReplacementTableName(value,&newname)) {
			node->setAttributeValue(sqlparser::_value,newname);
		}
	}

	// if the current node is an index name
	// then see if it needs to be replaced
	if (!charstring::compare(node->getName(),sqlparser::_index_name)) {

		const char	*newname=NULL;
		const char	*value=node->getAttributeValue(
						sqlparser::_value);

		if (sqlts->getReplacementIndexName(value,&newname)) {
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
