// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>

bool sqlparser::parseDrop(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a drop clause
	if (!dropClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*dropnode=newNode(currentnode,_drop);

	// temporary
	parseDropTemporary(dropnode,*newptr,newptr);

	// table, index, etc.
	if (parseDropTable(dropnode,*newptr,newptr) ||
		parseDropIndex(dropnode,*newptr,newptr)) {
		return true;
	}

	// for now we only support tables
	parseRemainderVerbatim(dropnode,*newptr,newptr);
	return true;
}

bool sqlparser::dropClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"drop ");
}

const char *sqlparser::_drop="drop";

bool sqlparser::parseDropTemporary(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	if (!temporaryClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_drop_temporary);
	return true;
}

const char *sqlparser::_drop_temporary="drop_temporary";

bool sqlparser::parseDropTable(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// table
	if (!tableClause(ptr,newptr)) {
		return false;
	}

	// create new node
	xmldomnode	*tablenode=newNode(currentnode,_table);

	// if not exists
	parseIfExists(tablenode,*newptr,newptr);

	// table names
	if (!parseTableNameList(tablenode,*newptr,newptr)) {
		return false;
	}

	// restrict
	parseRestrict(tablenode,*newptr,newptr);

	// cascade
	parseCascade(tablenode,*newptr,newptr);

	// store anything trailing off the end verbatim
	parseRemainderVerbatim(tablenode,*newptr,newptr);

	return true;
}

bool sqlparser::parseIfExists(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!ifExistsClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_if_exists);
	return true;
}

bool sqlparser::ifExistsClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"if exists ");
}

const char *sqlparser::_if_exists="if_exists";

bool sqlparser::parseTableNameList(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create the node
	xmldomnode	*tablesnode=newNode(currentnode,_table_name_list);

	*newptr=ptr;
	for (;;) {

		// create the node
		xmldomnode	*tablenode=
				newNode(tablesnode,_table_name_list_item);

		// get the table name
		if (!parseTableName(tablenode,*newptr,newptr)) {
			return false;
		}

		// if there's a comma afterward then we have more
		// table names to get, otherwise we're done
		if (!comma(*newptr,newptr)) {
			return true;
		}
	}
}

const char *sqlparser::_table_name_list="table_name_list";
const char *sqlparser::_table_name_list_item="table_name_list_item";

bool sqlparser::parseRestrict(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!restrictClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_restrict);
	return true;
}

bool sqlparser::restrictClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"restrict");
}

const char *sqlparser::_restrict="restrict";

bool sqlparser::parseCascade(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// cascade
	if (!cascadeClause(ptr,newptr)) {
		return false;
	}

	// create node
	xmldomnode	*cascadenode=newNode(currentnode,_cascade);

	// look for constraints
	parseCascadeConstraintsClause(cascadenode,*newptr,newptr);
	return true;
}

bool sqlparser::cascadeClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"cascade");
}

const char *sqlparser::_cascade="cascade";

bool sqlparser::parseCascadeConstraintsClause(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!cascadeConstraintsClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_cascade_constraints_clause);
	return true;
}

bool sqlparser::cascadeConstraintsClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"constraints");
}

const char *sqlparser::_cascade_constraints_clause="cascade_constraints_clause";

bool sqlparser::parseDropIndex(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// index
	if (!indexClause(ptr,newptr)) {
		return false;
	}

	// create new node
	xmldomnode	*indexnode=newNode(currentnode,_index);

	// index name
	if (!parseIndexName(indexnode,*newptr,newptr)) {
		return false;
	}

	// on
	parseOnClause(indexnode,*newptr,newptr);

	// table name
	parseTableName(indexnode,*newptr,newptr);

	return true;
}
