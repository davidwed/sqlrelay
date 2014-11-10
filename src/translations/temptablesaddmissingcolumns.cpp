// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <sqlrelay/sqlreparser.h>
#include <debugprint.h>

class temptablesaddmissingcolumns : public sqlrtranslation {
	public:
			temptablesaddmissingcolumns(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
};

temptablesaddmissingcolumns::temptablesaddmissingcolumns(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool temptablesaddmissingcolumns::usesTree() {
	return true;
}

bool temptablesaddmissingcolumns::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldom *querytree) {
	debugFunction();

	xmldomnode	*query=querytree->getRootNode();

	// ignore non create-table queries
	xmldomnode	*table=
			query->getFirstTagChild(sqlreparser::_create)->
				getFirstTagChild(sqlreparser::_table);
	if (table->isNullNode()) {
		return true;
	}

	// ignore non-temporary tables
	xmldomnode	*temporary=
			query->getFirstTagChild(sqlreparser::_create)->
				getFirstTagChild(sqlreparser::_temporary);
	if (temporary->isNullNode()) {
		return true;
	}

	// if there's already an columns clause then leave it alone
	xmldomnode	*columns=
			table->getFirstTagChild(sqlreparser::_columns);
	if (!columns->isNullNode()) {
		return true;
	}

	// get the as clause, bail if it is missing
	xmldomnode	*as=
			table->getFirstTagChild(sqlreparser::_as);
	if (as->isNullNode()) {
		return true;
	}

	// get the select clause, bail if it is missing
	xmldomnode	*select=
			table->getFirstTagChild(sqlreparser::_select);
	if (select->isNullNode()) {
		return true;
	}
	stringbuffer	selectclause;
	sqlreparser	sqlp;
	if (!sqlp.write(select,&selectclause,false)) {
		return true;
	}

	// get the columns
	stringbuffer	collist;
	if (!sqlrcon->cont->getColumnNames(selectclause.getString(),&collist)) {
		debugPrintf("failed to get column list\n");
	}

	// insert the columns themselves, if there are any
	char		**parts=NULL;
	uint64_t	partscount=0;
	charstring::split(collist.getString(),",",true,&parts,&partscount);

	if (partscount) {

		// create the columns node
		columns=sqlts->newNodeBefore(table,as,sqlreparser::_columns);

		for (uint64_t i=0; i<partscount; i++) {
			xmldomnode	*column=
					sqlts->newNode(columns,"column");
			sqlts->newNode(column,"name",parts[i]);
			delete[] parts[i];
		}

	}
	delete[] parts;

	return true;
}

extern "C" {
	sqlrtranslation	*new_temptablesaddmissingcolumns(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new temptablesaddmissingcolumns(sqlts,parameters,debug);
	}
}
