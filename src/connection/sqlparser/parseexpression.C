// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>
#include <rudiments/snooze.h>

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
		// (there could be any number of them, in any order)
		while (parseUnaryOperator(expressionnode,*newptr,newptr)) {
		}

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
		parseInverse(currentnode,ptr,newptr) ||
		parseNegative(currentnode,ptr,newptr));
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

bool sqlparser::parseNegative(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!minus(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_negative);
	return true;
}

const char *sqlparser::_negative="negative";

bool sqlparser::parseBinaryOperator(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	return (parseTimes(currentnode,ptr,newptr) ||
		parseDividedBy(currentnode,ptr,newptr) ||
		parseModulo(currentnode,ptr,newptr) ||
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

bool sqlparser::parseModulo(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!modulo(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_modulo);
	return true;
}

const char *sqlparser::_modulo="modulo";

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

bool sqlparser::parseTerm(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// initialize the return value
	bool	retval=true;

	// get the next chunk
	char	*term=getVerbatim(ptr,newptr);

	// test for numbers, string literals and bind variables...
	char	c=term[0];
	if (charstring::isNumber(term)) {
		newNode(currentnode,_number,term);
	} else if (c=='\'' || c=='"') {
		newNode(currentnode,_string_literal,term);
	} else if (c=='?' || c==':' || (c=='@' && term[1]!='@') || c=='$') {
		newNode(currentnode,_bind_variable,term);
	} else {

		// we'll need to do more to determine if
		// it's a function call or just a column
		if (!parseColumnOrFunction(currentnode,term,*newptr,newptr)) {

			// if it wasn't either then reset the
			// output pointer and return failure
			*newptr=ptr;
			retval=false;
		}
	}

	// clean up
	delete[] term;

	return retval;
}

const char *sqlparser::_number="number";
const char *sqlparser::_string_literal="string_literal";
const char *sqlparser::_bind_variable="bind_variable";

bool sqlparser::parseColumnOrFunction(xmldomnode *currentnode,
						const char *name,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	

	// functions generally have parameters or at least empty parameters
	if (leftParen(ptr,newptr)) {

		// FIXME: split dot-delimited function names

		// create the nodes
		xmldomnode	*functionnode=
				newNode(currentnode,_function,name);
		xmldomnode	*paramsnode=
				newNode(functionnode,_parameters);

		// parse parameters
		for (;;) {

			// bail when we find a right paren
			if (rightParen(*newptr,newptr)) {
				return true;
			}

			// create the node
			xmldomnode	*paramnode=
					newNode(paramsnode,_parameter,name);

			// parse the expression
			parseExpression(paramnode,*newptr,newptr);

			// skip any commas
			comma(*newptr,newptr);
		}
	}

	// it's either a special function without parameters or a column name...

	// FIXME: split dot-delimited function names

	const char	*type=_column_reference;

	// some db's have special functions with no parameters
	// (eg. sysdate, current_date, today, etc.)
	// if it's not one of those then it's just a regular column
	if (specialFunctionName(name)) {
		type=_function;
	} else {
		type=_column_reference;
	}

	// create the nodes
	xmldomnode	*newnode=newNode(currentnode,type);
	newNode(newnode,_name,name);
	return true;
}

const char *sqlparser::_column_reference="column_reference";
const char *sqlparser::_function="function";
const char *sqlparser::_parameters="parameters";
const char *sqlparser::_parameter="parameter";

static const char *defaultspecialfunctionnames[]={
	"sysdate",
	"current_date",
	NULL
};

bool sqlparser::specialFunctionName(const char *name) {

	// check the default list
	const char * const	*names=defaultspecialfunctionnames;
	for (uint64_t i=0; names[i]; i++) {
		if (!charstring::compare(name,names[i])) {
			return true;
		}
	}

	// check the db-specific list
	names=specialFunctionNames();
	if (names) {
		for (uint64_t i=0; names[i]; i++) {
			if (!charstring::compare(name,names[i])) {
				return true;
			}
		}
	}
	return false;
}

const char * const *sqlparser::specialFunctionNames() {
	return NULL;
}
