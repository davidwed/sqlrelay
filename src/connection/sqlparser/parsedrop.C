// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqlelement.h>
#include <sqltranslatordebug.h>

bool sqlparser::parseDrop(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	*newptr=ptr;

	// look for a drop clause
	if (!dropClause(*newptr,newptr)) {
		debugPrintf("no drop clause\n");
		return false;
	}

	// create the node
	xmldomnode	*dropnode=
			newNode(currentnode,sqlelement::_drop);

	// temporary
	parseDropTemporary(dropnode,*newptr,newptr);

	// table, index, etc.
	if (tableClause(*newptr,newptr)) {
		return parseDropTable(dropnode,*newptr,newptr);
	}

	// for now we only support tables
	parseRemainderVerbatim(dropnode,*newptr,newptr);
	return true;
}

bool sqlparser::parseDropTemporary(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	if (!temporaryClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_drop_temporary);
	return true;
}

bool sqlparser::parseDropTable(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create new node
	xmldomnode	*tablenode=newNode(currentnode,sqlelement::_table);

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
	newNode(currentnode,sqlelement::_if_exists);
	return true;
}

bool sqlparser::parseTableNameList(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create the node
	xmldomnode	*tablesnode=
			newNode(currentnode,sqlelement::_table_name_list);

	*newptr=ptr;
	for (;;) {

		// create the node
		xmldomnode	*tablenode=
			newNode(tablesnode,sqlelement::_table_name_list_item);

		// get the table name
		if (!parseTableName(tablenode,*newptr,newptr)) {
			return false;
		}

		// if we hit the end of the query then we're done
		if (!**newptr) {
			return true;
		}

		// if there's a space afterward, we're done getting table names
		// (we have to check 1 char back because the space would have
		// been consumed by the parseTableName call above)
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

bool sqlparser::parseRestrict(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!restrictClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_restrict);
	return true;
}

bool sqlparser::parseCascade(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!cascadeClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_cascade);
	return true;
}
