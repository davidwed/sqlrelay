// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqltranslatordebug.h>

bool sqlparser::parseInsert(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a insert clause
	if (!insertClause(ptr,newptr)) {
		debugPrintf("no insert clause\n");
		return false;
	}

	// create the node
	xmldomnode	*insertnode=newNode(currentnode,_insert);

	// parse the insert clauses
	for (;;) {

		// look for the into clause
		if (parseInsertInto(insertnode,*newptr,newptr)) {
			break;
		}

		// If we didn't encounter one of the known clauses
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (parseVerbatim(insertnode,*newptr,newptr)) {
			space(*newptr,newptr);
		} else {
			break;
		}
	}

	// "values" or "value" clause
	if (!parseInsertValues(insertnode,*newptr,newptr) &&
		!parseInsertValue(insertnode,*newptr,newptr) &&
		!parseUpdateSet(insertnode,*newptr,newptr) &&
		!parseSelect(insertnode,*newptr,newptr)) {
		debugPrintf("missing value, values, set or select clause\n");
		return false;
	}

	parseRemainderVerbatim(insertnode,*newptr,newptr);
	return true;
}

bool sqlparser::insertClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"insert ");
}

const char *sqlparser::_insert="insert";

bool sqlparser::parseInsertInto(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// into
	if (!insertIntoClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*intonode=newNode(currentnode,_insert_into);

	// table name
	if (!parseName(intonode,*newptr,newptr)) {
		debugPrintf("missing table name\n");
		return false;
	}

	// columns (optional)
	if (leftParen(*newptr,newptr)) {

		// parse columns
		if (!parseColumnNameList(intonode,*newptr,newptr)) {
			return false;
		}

		// right paren
		if (!rightParen(*newptr,newptr)) {
			return false;
		}
	}

	// space
	space(*newptr,newptr);
	return true;
}

bool sqlparser::insertIntoClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"into ");
}

const char *sqlparser::_insert_into="insert_into";

bool sqlparser::parseInsertValues(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// values
	if (!insertValuesClause(ptr,newptr)) {
		return false;
	}
	
	// create the node
	xmldomnode	*valuesnode=newNode(currentnode,_insert_values);

	// the values themselves
	return parseInsertValuesList(valuesnode,*newptr,newptr);
}

bool sqlparser::insertValuesClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"values ");
}

const char *sqlparser::_insert_values="insert_values";

bool sqlparser::parseInsertValue(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// value
	if (!insertValueClause(ptr,newptr)) {
		return false;
	}
	
	// create the node
	xmldomnode	*valuenode=newNode(currentnode,_insert_value);

	// the values themselves
	return parseInsertValuesList(valuenode,*newptr,newptr);
}

bool sqlparser::insertValueClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"value ");
}

const char *sqlparser::_insert_value="insert_value";

bool sqlparser::parseInsertValuesList(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// left paren
	if (!leftParen(ptr,newptr)) {
		return false;
	}

	// run through the values
	for (;;) {

		// if we hit the end of the string then there was a problem
		if (!**newptr) {
			return false;
		}

		// if we find a right paren then we're done
		if (rightParen(*newptr,newptr)) {
			return true;
		}

		// this could be an expression, but for now,
		// get the value verbatim
		char	*value=getUntil(",)",*newptr,newptr);
		newNode(currentnode,_value,value);
		delete[] value;

		// skip the comma, if there was one
		comma(*newptr,newptr);
	}
}
