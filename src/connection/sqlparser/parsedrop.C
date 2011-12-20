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
		debugPrintf("no drop clause\n");
		return false;
	}

	// create the node
	xmldomnode	*dropnode=newNode(currentnode,_drop);

	// temporary
	parseDropTemporary(dropnode,*newptr,newptr);

	// table, index, etc.
	if (comparePart(*newptr,newptr,"table ")) {
		return parseDropTable(dropnode,*newptr,newptr);
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

	// create new node
	xmldomnode	*tablenode=newNode(currentnode,_table);

	// if not exists
	parseIfExists(tablenode,ptr,newptr);

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
		if (!parseName(tablenode,*newptr,newptr)) {
			return false;
		}

		// if we hit the end of the query then we're done
		if (!**newptr) {
			return true;
		}

		// if there's a space afterward, we're done getting table names
		// (we have to check 1 char back because the space would have
		// been consumed by the parseName call above)
		if (*(*newptr-1)==' ') {
			return true;
		}

		// if there's a comma afterward then
		// we have more table names to get
		if (!comma(*newptr,newptr)) {
			debugPrintf("missing comma or space\n");
			return false;
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
	if (!cascadeClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_cascade);
	return true;
}

bool sqlparser::cascadeClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"cascade",
		"cascade constraints",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlparser::_cascade="cascade";
