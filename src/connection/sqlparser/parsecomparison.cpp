// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>

bool sqlparser::parseComparison(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					bool checkforgroup) {
	debugFunction();

	// create the node
	xmldomnode	*comparisonnode=newNode(currentnode,_comparison);

	// look for not's
	if (notClause(ptr,newptr)) {

		// create the node
		newNode(comparisonnode,_not);
	}

	// handle exists
	if (parseExists(comparisonnode,*newptr,newptr)) {
		return true;
	}

	// get the lvalue
	const char	*beforeexpr=*newptr;
	if (!parseExpression(comparisonnode,*newptr,newptr)) {

		// If the expression failed to parse, there might be parens
		// around the entire comparison.  Look for a left paren,
		// if we don't find it then something is wrong.
		*newptr=beforeexpr;
		if (!leftParen(*newptr,newptr)) {
			debugPrintf("missing lvalue\n");
			error=true;
			return false;
		}
	
		// create the node
		xmldomnode	*groupnode=new xmldomnode(tree,
						comparisonnode->getNullNode(),
						TAG_XMLDOMNODETYPE,
						_group,NULL);

		// parse the comparison
		if (parseComparison(groupnode,*newptr,newptr,false) &&
					rightParen(*newptr,newptr)) {
			comparisonnode->appendChild(groupnode);
			return true;
		}

		// If this failed to parse then the paren we ran into might
		// have been part of a group of where clause terms.  Clean up
		// and bail so we can start over from there.
		*newptr=beforeexpr;
		delete groupnode;
		return false;
	}

	// look for not's again
	if (notClause(*newptr,newptr)) {

		// create the node
		newNode(comparisonnode,_not);
	}

	// handle betweens
	if (parseBetween(comparisonnode,*newptr,newptr)) {
		return true;
	}

	// handle in
	if (parseIn(comparisonnode,*newptr,newptr)) {
		return true;
	}

	// get the comparator
	if (parseIs(comparisonnode,*newptr,newptr) ||
		parseLike(comparisonnode,*newptr,newptr) ||
		parseNullSafeEquals(comparisonnode,*newptr,newptr) ||
		parseEquals(comparisonnode,*newptr,newptr) ||
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

	// if the term was a boolean value or function returning a boolean
	// then there might not be a comparator
	return true;
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

bool sqlparser::parseIn(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for in
	if (!inClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*innode=newNode(currentnode,_in);

	// there should be a left paren
	if (!leftParen(*newptr,newptr)) {
		debugPrintf("missing left paren\n");
		error=true;
		return false;
	}

	// parse the select
	if (!parseSelect(innode,*newptr,newptr) &&
		!parseInSet(innode,*newptr,newptr)) {
		debugPrintf("missing select\n");
		error=true;
		return false;
	}

	// there should be a right paren
	if (!rightParen(*newptr,newptr)) {
		debugPrintf("missing right paren\n");
		error=true;
		return false;
	}

	return false;
}

bool sqlparser::inClause(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"in ");
}                                   

const char *sqlparser::_in="in";

bool sqlparser::parseInSet(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	*newptr=ptr;
	for (;;) {

		// create the node
		xmldomnode	*insetitemnode=
				newNode(currentnode,_in_set_item);

		// get the expression, bail when we don't find one
		if (!parseExpression(insetitemnode,*newptr,newptr)) {
			debugPrintf("missing expression\n");
			error=true;
			return false;
		}

		// get the comma after the expression
		comma(*newptr,newptr);

		// if we find a right paren then we're done
		const char 	*before=*newptr;
		if (rightParen(*newptr,newptr)) {
			*newptr=before;
			return true;
		}
	}
}

const char *sqlparser::_in_set_item="in_set_item";

bool sqlparser::parseExists(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for exists
	if (!existsClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*existsnode=newNode(currentnode,_exists);

	// there should be a left paren
	if (!leftParen(*newptr,newptr)) {
		debugPrintf("missing left paren\n");
		error=true;
		return false;
	}

	// parse the select
	if (!parseSelect(existsnode,*newptr,newptr)) {
		debugPrintf("missing select\n");
		error=true;
		return false;
	}

	// there should be a right paren
	if (!rightParen(*newptr,newptr)) {
		debugPrintf("missing right paren\n");
		error=true;
		return false;
	}

	return true;
}

bool sqlparser::existsClause(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"exists");
}                                   

const char *sqlparser::_exists="exists";

bool sqlparser::parseIs(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!is(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_is);
	return true;
}

bool sqlparser::is(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"is ");
}                                   

const char *sqlparser::_is="is";

bool sqlparser::parseLike(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!like(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_like);
	return true;
}

bool sqlparser::like(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"like ");
}                                   

const char *sqlparser::_like="like";

bool sqlparser::parseNullSafeEquals(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!nullSafeEquals(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_null_safe_equals);
	return true;
}

bool sqlparser::nullSafeEquals(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"<=>");
}                                   

const char *sqlparser::_null_safe_equals="null_safe_equals";

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
