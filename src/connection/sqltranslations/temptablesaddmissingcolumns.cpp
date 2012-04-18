// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltranslations/temptablesaddmissingcolumns.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>

temptablesaddmissingcolumns::temptablesaddmissingcolumns(
					sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool temptablesaddmissingcolumns::run(sqlrconnection_svr *sqlrcon,
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
	columns=sqlts->newNodeBefore(table,as,sqlparser::_columns);

	// insert the columns themselves
	char		**parts;
	uint64_t	partscount;
	charstring::split(collist.getString(),",",true,&parts,&partscount);
	for (uint64_t i=0; i<partscount; i++) {
		xmldomnode	*column=sqlts->newNode(columns,"column");
		sqlts->newNode(column,"name",parts[i]);
	}
	for (uint64_t i=0; i<partscount; i++) {
		delete[] parts[i];
	}
	delete[] parts;

	return true;
}
