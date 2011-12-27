// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#define DEBUG_MESSAGES
#include <debugprint.h>

bool sqlparser::parseComparison(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create the node
	xmldomnode	*comparisonnode=newNode(currentnode,_comparison);

	// look for not's
	*newptr=ptr;
	if (notClause(*newptr,newptr)) {

		// create the node
		xmldomnode	*notnode=newNode(comparisonnode,_not);

		// parse the comparison
		return parseComparison(notnode,*newptr,newptr);
	}

	// get the lvalue
	if (!parseExpression(comparisonnode,*newptr,newptr)) {
		debugPrintf("missing lvalue\n");
		error=true;
		return false;
	}

	// if we have a between clause then there will be a space before it
	space(*newptr,newptr);

	// handle betweens
	if (parseBetween(comparisonnode,*newptr,newptr)) {
		return true;
	}

	// get the comparator
	if (parseEquals(comparisonnode,*newptr,newptr) ||
		parseNotEquals(comparisonnode,*newptr,newptr) ||
		parseGreaterThanOrEqualTo(comparisonnode,*newptr,newptr) ||
		parseLessThanOrEqualTo(comparisonnode,*newptr,newptr) ||
		parseGreaterThan(comparisonnode,*newptr,newptr) ||
		parseLessThan(comparisonnode,*newptr,newptr)) {

		// get the rvalue
		if (!parseExpression(comparisonnode,*newptr,newptr)) {
			debugPrintf("missing rvalue\n");
			error=true;
			return false;
		}
		return true;
	}

	debugPrintf("missing comparator\n");
	error=true;
	return false;
}

const char *sqlparser::_comparison="comparison";

bool sqlparser::notClause(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"not ");
}                                   

const char *sqlparser::_not="not";

bool sqlparser::parseBetween(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for between
	if (!betweenClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*betweennode=newNode(currentnode,_between);

	// expression and expression
	if (!parseExpression(betweennode,*newptr,newptr)) {
		debugPrintf("missing expression\n");
		error=true;
		return false;
	}
	space(*newptr,newptr);
	if (!parseAnd(betweennode,*newptr,newptr)) {
		debugPrintf("missing and\n");
		error=true;
		return false;
	}
	if (!parseExpression(betweennode,*newptr,newptr)) {
		debugPrintf("missing expression\n");
		error=true;
		return false;
	}
	return true;
}

bool sqlparser::betweenClause(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"between ");
}                                   

const char *sqlparser::_between="between";

bool sqlparser::parseEquals(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!equals(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_equals);
	return true;
}

bool sqlparser::parseNotEquals(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!notEquals(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_not_equals);
	return true;
}
                   
const char *sqlparser::_not_equals="notequals";

bool sqlparser::parseLessThan(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!lessThan(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_less_than);
	return true;
}

const char *sqlparser::_less_than="lessthan";

bool sqlparser::parseGreaterThan(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!greaterThan(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_greater_than);
	return true;
}

const char *sqlparser::_greater_than="greaterthan";

bool sqlparser::parseLessThanOrEqualTo(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!lessThanOrEqualTo(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_less_than_or_equal_to);
	return true;
}

const char *sqlparser::_less_than_or_equal_to="lessthanorequalto";

bool sqlparser::parseGreaterThanOrEqualTo(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!greaterThanOrEqualTo(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_greater_than_or_equal_to);
	return true;
}

const char *sqlparser::_greater_than_or_equal_to="greaterthanorequalto";
