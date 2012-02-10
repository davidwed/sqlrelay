// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <oracle8sqltranslator.h>
#include <sqlparser.h>
#include <sqlwriter.h>
#include <debugprint.h>
#include <sqlrconnection.h>

oracle8sqltranslator::oracle8sqltranslator() : sqltranslator() {
	debugFunction();
}

oracle8sqltranslator::~oracle8sqltranslator() {
	debugFunction();
}

bool oracle8sqltranslator::applyRulesToQuery(xmldomnode *query) {
	debugFunction();

	for (xmldomnode *rule=rulesnode->getFirstTagChild();
		!rule->isNullNode(); rule=rule->getNextTagSibling()) {

		const char	*rulename=rule->getName();

		if (!charstring::compare(rulename,
				"temp_tables_preserve_rows_by_default")) {
			if (!tempTablesPreserveRowsByDefault(query,rule)) {
				return false;
			}
		} else if (!charstring::compare(rulename,
				"temp_tables_add_missing_columns")) {
			if (!tempTablesAddMissingColumns(query,rule)) {
				return false;
			}
		}
	}

	return sqltranslator::applyRulesToQuery(query);
}

bool oracle8sqltranslator::translateDatatypes(xmldomnode *query,
						xmldomnode *rule) {
	debugFunction();
	return true;
}

bool oracle8sqltranslator::tempTablesPreserveRowsByDefault(
						xmldomnode *query,
						xmldomnode *rule) {
	debugFunction();

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
	oncommit=newNodeAfter(table,columns,sqlparser::_on_commit);
	setAttribute(oncommit,sqlparser::_value,"preserve rows");
	return true;
}

bool oracle8sqltranslator::tempTablesAddMissingColumns(
						xmldomnode *query,
						xmldomnode *rule) {
	debugFunction();

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

	// if there's already an columns clause then leave it alone
	xmldomnode	*columns=
			table->getFirstTagChild(sqlparser::_columns);
	if (!columns->isNullNode()) {
		return true;
	}

	// get the as clause, bail if it is missing
	xmldomnode	*as=
			table->getFirstTagChild(sqlparser::_as);
	if (as->isNullNode()) {
		return true;
	}

	// get the select clause, bail if it is missing
	xmldomnode	*select=
			table->getFirstTagChild(sqlparser::_select);
	if (select->isNullNode()) {
		return true;
	}
	stringbuffer	selectclause;
	sqlwriter	sqlw;
	if (!sqlw.write(sqlrcon,sqlrcur,select,&selectclause,false)) {
		return true;
	}

	// get the columns
	stringbuffer	collist;
	if (!sqlrcon->getColumnNames(selectclause.getString(),&collist)) {
		debugPrintf("failed to get column list\n");
	}

	// create the columns node
	columns=newNodeBefore(table,as,sqlparser::_columns);

	// insert the columns themselves
	char		**parts;
	uint64_t	partscount;
	charstring::split(collist.getString(),",",true,&parts,&partscount);
	for (uint64_t i=0; i<partscount; i++) {
		xmldomnode	*column=newNode(columns,"column");
		newNode(column,"name",parts[i]);
	}
	for (uint64_t i=0; i<partscount; i++) {
		delete[] parts[i];
	}
	delete[] parts;

	return true;
}
