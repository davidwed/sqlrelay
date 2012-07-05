// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
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
		// (there could be any number of them, in any order)
		do {} while (parseUnaryOperator(expressionnode,*newptr,newptr));

		// handle expression groups
		const char	*beforeparen=*newptr;
		if (leftParen(*newptr,newptr)) {

			// create the node
			xmldomnode	*groupnode=new xmldomnode(tree,
						expressionnode->getNullNode(),
						TAG_XMLDOMNODETYPE,
						_group,NULL);

			// parse the expression inside the parens
			if (!parseExpression(groupnode,*newptr,newptr)) {
				return false;
			}

			// look for a right paren
			if (!rightParen(*newptr,newptr)) {
				*newptr=beforeparen;
				delete groupnode;
				/*debugPrintf("missing right paren\n");	
				error=true;*/
				return false;
			}

			// append the node
			expressionnode->appendChild(groupnode);

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
	return (parseNot(currentnode,ptr,newptr) ||
		parseDistinct(currentnode,ptr,newptr) ||
		parseCompliment(currentnode,ptr,newptr) ||
		parseInverse(currentnode,ptr,newptr) ||
		parseNegative(currentnode,ptr,newptr));
}

bool sqlparser::parseNot(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!notClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_not);
	return true;
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

	// first, check to see if it's an interval qualifier
	if (parseIntervalQualifier(currentnode,ptr,newptr)) {
		return true;
	}

	// initialize the return value
	bool	retval=true;

	// Get the next chunk.  If it ends in a . then get the one after
	// because it's probably a * and they need to be stuck together.
	char	*term=getVerbatim(ptr,newptr);
	if (term[charstring::length(term)-1]=='.') {
		const char	*before=*newptr;
		char	*nextterm=getVerbatim(*newptr,newptr);
		if (!charstring::compare(nextterm,"*")) {
			stringbuffer	combined;
			combined.append(term)->append(nextterm);
			delete[] term;
			term=combined.detachString();
		} else {
			*newptr=before;
		}
		delete[] nextterm;
	}

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

bool sqlparser::parseIntervalQualifier(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create the node
	xmldomnode	*iqnode=new xmldomnode(tree,
					currentnode->getNullNode(),
					TAG_XMLDOMNODETYPE,
					_interval_qualifier,NULL);

	// look for a time component, to, and another time component
	bool	retval=parseTimeComponent(iqnode,ptr,newptr,
						_first_time_component,
						_precision) &&
			parseTo(iqnode,*newptr,newptr) &&
			parseTimeComponent(iqnode,*newptr,newptr,
						_second_time_component,
						_scale);

	// if everything went well, attach the node,
	// otherwise reset the string pointer and delete it
	if (retval) {
		currentnode->appendChild(iqnode);
	} else {
		*newptr=ptr;
		delete iqnode;
	}

	return retval;
}

const char *sqlparser::_interval_qualifier="interval_qualifier";

bool sqlparser::parseTo(xmldomnode *currentnode,
				const char *ptr,
				const char **newptr) {
	debugFunction();
	return toClause(ptr,newptr);
}

bool sqlparser::toClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"to ");
}

const char *sqlparser::_to="to";

bool sqlparser::parseTimeComponent(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					const char *timecomponent,
					const char *precscale) {
	debugFunction();

	// verify the time component
	const char *parts[]={
		"day",
		"month",
		"year",
		"hour",
		"minute",
		"second",
		"fraction",
		NULL
	};
	if (!comparePart(ptr,newptr,parts)) {
		return false;
	}

	// back up, get it and add it as an attribute
	*newptr=ptr;
	char	*part=getVerbatim(*newptr,newptr);
	currentnode->setAttributeValue(timecomponent,part);
	delete[] part;

	// check for precision/scale
	if (!leftParen(*newptr,newptr)) {
		return true;
	}
	char	*number=getVerbatim(*newptr,newptr);
	if (charstring::isNumber(number)) {
		currentnode->setAttributeValue(precscale,number);
		delete[] number;
	} else {
		delete[] number;
		return false;
	}
	return rightParen(*newptr,newptr);
}

const char *sqlparser::_first_time_component="first_time_component";
const char *sqlparser::_second_time_component="second_time_component";
const char *sqlparser::_precision="precision";

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
					newNode(paramsnode,_parameter);

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
	if (type==_function) {
		newNode(newnode,_name,name);
	} else {
		splitColumnName(newnode,name);
	}
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

void sqlparser::splitColumnName(xmldomnode *currentnode, const char *name) {

	// split the name
	char		**parts;
	uint64_t	count;
	charstring::split(name,".",true,&parts,&count);

	char	*db=NULL;
	char	*schema=NULL;
	char	*table=NULL;
	char	*column=NULL;

	// combine initial parts into db name
	uint64_t	start=0;
	if (count>4) {
		stringbuffer	dbstr;
		for (start=0; start<count-3; start++) {
			if (start) {
				dbstr.append('.');
			}
			dbstr.append(parts[start]);
		}
		db=dbstr.detachString();
	} else if (count>3) {
		db=parts[start++];
	}

	// set schema, table, column names
	if (count>2) {
		schema=parts[start++];
	}
	if (count>1) {
		table=parts[start++];
	}
	column=parts[start];

	// create nodes for each part
	if (db) {
		newNode(currentnode,_column_name_database,db);
	}
	if (schema) {
		newNode(currentnode,_column_name_schema,schema);
	}
	if (table) {
		newNode(currentnode,_column_name_table,table);
	}
	if (column) {
		newNode(currentnode,_column_name_column,column);
	}

	// clean up
	for (uint64_t i=0; i<count; i++) {
		delete[] parts[i];
	}
	delete[] parts;
}

const char *sqlparser::_column_name_database="column_name_database";
const char *sqlparser::_column_name_schema="column_name_schema";
const char *sqlparser::_column_name_table="column_name_table";
const char *sqlparser::_column_name_column="column_name_column";
