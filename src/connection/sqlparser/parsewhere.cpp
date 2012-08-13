// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>	
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

	// parse the where clause terms
	return parseWhereClauseTerms(wherenode,*newptr,newptr);
}

bool sqlparser::whereClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"where ");
}

const char *sqlparser::_where="where";

bool sqlparser::parseHaving(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a having clause
	if (!havingClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*havingnode=newNode(currentnode,_having);

	// parse the having clause terms
	return parseWhereClauseTerms(havingnode,*newptr,newptr);
}

bool sqlparser::havingClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"having ");
}

const char *sqlparser::_having="having";

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

	// handle single comparisons
	if (parseComparison(currentnode,ptr,newptr,true)) {
		return true;
	}

	// If the comparison failed to parse, it might be a where clause group.
	// Look for a left paren.  If we don't find it then something is wrong.
	*newptr=ptr;
	if (!leftParen(*newptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*groupnode=new xmldomnode(tree,
					currentnode->getNullNode(),
					TAG_XMLDOMNODETYPE,
					_group,NULL);

	// parse where clause terms and look for a right paren
	if (parseWhereClauseTerms(groupnode,*newptr,newptr) &&
					rightParen(*newptr,newptr)) {
		currentnode->appendChild(groupnode);
		return true;
	}

	// If this failed to parse then it's really not clear what we've got.
	debugPrintf("extraneous left paren\n");
	error=true;
	return false;
}

const char *sqlparser::_group="group";
