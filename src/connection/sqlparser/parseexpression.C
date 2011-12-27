// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#define DEBUG_MESSAGES
#include <debugprint.h>

bool sqlparser::parseExpression(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create the node
	xmldomnode	*expressionnode=newNode(currentnode,_expression);

	// any number of terms, separated by operators
	*newptr=ptr;
	for (;;) {

		// handle any unary operators
		parseUnaryOperator(expressionnode,*newptr,newptr);

		// handle expression groups
		if (leftParen(*newptr,newptr)) {

			// create the node
			xmldomnode	*groupnode=newNode(
						expressionnode,_group);

			// parse the expression inside the parens
			if (!parseExpression(groupnode,*newptr,newptr)) {
				return false;
			}

			// look for a right paren
			if (!rightParen(*newptr,newptr)) {
				debugPrintf("missing right paren\n");	
				error=true;
				return false;
			}

		} else {

			// get the term
			if (!parseTerm(expressionnode,*newptr,newptr)) {
				return false;
			}
		}

		// get the operator
		if (!parseBinaryOperator(expressionnode,*newptr,newptr)) {
			// if there isn't one then we've reached the end of
			// the expression
			return true;
		}
	}
}

const char *sqlparser::_expression="expression";

bool sqlparser::parseUnaryOperator(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	return (parseCompliment(currentnode,ptr,newptr) ||
		parseInverse(currentnode,ptr,newptr));
}

bool sqlparser::parseCompliment(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!compliment(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_compliment);
	return true;
}

const char *sqlparser::_compliment="compliment";

bool sqlparser::parseInverse(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!inverse(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_inverse);
	return true;
}

const char *sqlparser::_inverse="inverse";

bool sqlparser::parseTerm(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	// FIXME: implement this for real
	char	*value=getVerbatim(ptr,newptr);
	newNode(currentnode,_verbatim,value);
	delete[] value;
	return true;
}

bool sqlparser::parseBinaryOperator(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	return (parseTimes(currentnode,ptr,newptr) ||
		parseDividedBy(currentnode,ptr,newptr) ||
		parsePlus(currentnode,ptr,newptr) ||
		parseMinus(currentnode,ptr,newptr) ||
		parseLogicalAnd(currentnode,ptr,newptr) ||
		parseLogicalOr(currentnode,ptr,newptr) ||
		parseBitwiseAnd(currentnode,ptr,newptr) ||
		parseBitwiseOr(currentnode,ptr,newptr) ||
		parseBitwiseXor(currentnode,ptr,newptr));
}

bool sqlparser::parseTimes(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!times(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_times);
	return true;
}

const char *sqlparser::_times="times";

bool sqlparser::parseDividedBy(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!dividedBy(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_divided_by);
	return true;
}

const char *sqlparser::_divided_by="divided_by";

bool sqlparser::parsePlus(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!plus(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_plus);
	return true;
}

const char *sqlparser::_plus="plus";

bool sqlparser::parseMinus(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!minus(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_minus);
	return true;
}

const char *sqlparser::_minus="minus";

bool sqlparser::parseLogicalAnd(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!logicalAnd(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_logical_and);
	return true;
}

const char *sqlparser::_logical_and="logical_and";

bool sqlparser::parseLogicalOr(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!logicalOr(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_logical_or);
	return true;
}

const char *sqlparser::_logical_or="logical_or";

bool sqlparser::parseBitwiseAnd(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!bitwiseAnd(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_bitwise_and);
	return true;
}

const char *sqlparser::_bitwise_and="bitwise_and";

bool sqlparser::parseBitwiseOr(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!bitwiseOr(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_bitwise_or);
	return true;
}

const char *sqlparser::_bitwise_or="bitwise_or";

bool sqlparser::parseBitwiseXor(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!bitwiseXor(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_bitwise_xor);
	return true;
}

const char *sqlparser::_bitwise_xor="bitwise_xor";
