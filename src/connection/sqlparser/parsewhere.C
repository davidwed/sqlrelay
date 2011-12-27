// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#define DEBUG_MESSAGES
#include <debugprint.h>

bool sqlparser::parseWhere(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a where clause
	if (!whereClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*wherenode=newNode(currentnode,_where);

// bail here to disable where-clause parsing
// return true;

	// parse the where clause terms
	return parseWhereClauseTerms(wherenode,*newptr,newptr);
}

bool sqlparser::whereClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"where ");
}

const char *sqlparser::_where="where";

bool sqlparser::parseWhereClauseTerms(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// parse the where clause terms
	bool	first=true;
	*newptr=ptr;
	for (;;) {

		// look for a where clause term
		if (!parseWhereClauseTerm(currentnode,*newptr,newptr)) {

			// there must be at least 1 where clause term
			if (first) {
				return false;
			}
			return true;
		}

		// eat up the space between the term and the and/or
		space(*newptr,newptr);

		// bail if it's not followed by an and or an or
		if (!parseAnd(currentnode,*newptr,newptr) &&
			!parseOr(currentnode,*newptr,newptr)) {
			return true;
		}

		first=false;
	}
}

bool sqlparser::parseAnd(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!andClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_and);
	return true;
}

bool sqlparser::andClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"and ");
}

const char *sqlparser::_and="and";

bool sqlparser::parseOr(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!orClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_or);
	return true;
}

bool sqlparser::orClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"or ");
}

const char *sqlparser::_or="or";

bool sqlparser::parseWhereClauseTerm(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// handle groups
// FIXME: problem here....
// it's possible that the left paren is just grouping terms on the left side of
// an expression, not grouping comparisons in the where clause
	*newptr=ptr;
	if (leftParen(*newptr,newptr)) {

		// create the node
		xmldomnode	*groupnode=newNode(currentnode,_group);

		// parse where clause terms
		if (!parseWhereClauseTerms(groupnode,*newptr,newptr)) {
			debugPrintf("missing terms inside parentheses\n");
			error=true;
			return false;
		}

		// look for a right paren
		return rightParen(*newptr,newptr);
	}

	// handle single comparisons
	return parseComparison(currentnode,*newptr,newptr);
}

const char *sqlparser::_group="group";
