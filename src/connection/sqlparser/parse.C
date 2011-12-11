// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqltranslatordebug.h>

bool sqlparser::comparePart(const char *ptr, const char **newptr,
							const char *part) {
	debugFunction();
	

	// get the part length
	uint64_t	length=charstring::length(part);

	#ifdef DEBUG_MESSAGES
	printf("\"");
	for (uint64_t i=0; i<length && ptr[i]; i++) {
		printf("%c",ptr[i]);
	}
	printf("\" == \"%s\"\n",part);
	#endif

	// see if the next "length" bytes are equal to the part
	if (!charstring::compareIgnoringCase(ptr,part,length)) {

		// if so then skip past the matching section
		*newptr=ptr+length;

		// the part matched, return success
		return true;
	}

	// the part did not match, return failure
	return false;
}

bool sqlparser::comparePart(const char *ptr, const char **newptr,
						const char * const *parts) {
	debugFunction();

	// run through the array of parts...
	for (uint64_t i=0; parts[i]; i++) {

		// compare against each of the parts
		if (comparePart(ptr,newptr,parts[i])) {

			// the part matched, return success
			return true;
		}
	}

	// the part did not match, return failure
	return false;
}

char *sqlparser::getWord(const char *ptr, const char **newptr) {
	debugFunction();

	// get the next block of whatever until we hit a space,
	// parenthesis, comma, operator or quotation
	return getUntil(" (,)~!^*-+=<>/&|'\"",ptr,newptr);
}

char *sqlparser::getClause(const char *ptr, const char *newptr) {
	return charstring::duplicate(ptr,newptr-ptr);
}

char *sqlparser::getUntil(const char *set,
				const char *ptr, const char **newptr) {
	debugFunction();

	// find the next space, comma or right parentheses
	const char	*end=charstring::findFirstOfSetOrEnd(ptr,set);

	// make a copy of the word we found
	char	*retval=charstring::duplicate(ptr,end-ptr);

	// set the return pointer
	*newptr=end;

	// if there's a space afterward, then bump past it
	space(*newptr,newptr);

	// return the word
	debugPrintf("getUntil: \"%s\"\n",retval);
	return retval;
}

static const char *verbatimTerminators=" ,)";

char *sqlparser::getVerbatim(const char *ptr, const char **newptr) {
	debugFunction();

	// declare a buffer to store the data
	stringbuffer	verbatim;

	// set state flags
	bool		inquotes=false;
	bool		indoublequotes=false;
	uint16_t	parens=0;

	// initialize the location
	const char	*chr=ptr;

	// run through the string...
	for (;;) {

		// break on end of string or space, comma or right paren
		// unless we're in quotes or parens
		if (!*chr ||
			(!inquotes && !indoublequotes && !parens && 
							inSet(*chr," ,)"))) {
			break;
		}

		// if we're in quotes and we hit an escape character
		// then blindly append the next character and move on
		if ((inquotes || indoublequotes) && *chr=='\\') {
			chr++;
			verbatim.append(*chr);
			chr++;
			continue;
		}

		// check for quotes, double quotes and parentheses
		if (!indoublequotes && *chr=='\'') {
			inquotes=!inquotes;
		} else if (!inquotes && *chr=='"') {
			indoublequotes=!indoublequotes;
		} else if (!inquotes && !indoublequotes && *chr=='(') {
			parens++;
		} else if (!inquotes && !indoublequotes && *chr==')') {
			parens--;
		}

		// append the character
		verbatim.append(*chr);

		// move on
		chr++;
	}

	// set output pointer
	*newptr=chr;

	// return the string
	debugPrintf("verbatim: \"%s\"\n",verbatim.getString());
	return verbatim.detachString();
}

bool sqlparser::parse(const char *query) {
	debugFunction();

	// create the tree
	delete tree;
	tree=new xmldom();
	tree->createRootNode();

	// parse the query
	char		*ptr=cleanQuery(query);
	const char	*newptr=ptr;
	bool	retval=parseQuery(tree->getRootNode(),ptr,&newptr);
	delete[] ptr;

	// return result
	return retval;
}

bool sqlparser::parseQuery(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	return parseCreate(currentnode,ptr,newptr) ||
		parseDrop(currentnode,ptr,newptr) ||
		parseInsert(currentnode,ptr,newptr) ||
		parseUpdate(currentnode,ptr,newptr) ||
		parseDelete(currentnode,ptr,newptr) ||
		parseSelect(currentnode,ptr,newptr);
}

bool sqlparser::parseName(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	char	*name=getWord(ptr,newptr);
	newNode(currentnode,_name,name);
	delete[] name;
	return true;
}

const char *sqlparser::_name="name";

bool sqlparser::parseType(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// get the type
	char	*type=getWord(ptr,newptr);
	xmldomnode	*typenode=newNode(currentnode,_type,type);

	// enum and set types have special characteristics
	bool	enumorset=(!charstring::compareIgnoringCase(type,"enum") ||
				!charstring::compareIgnoringCase(type,"set"));

	// clean up
	delete[] type;

	// get left paren
	if (!leftParen(*newptr,newptr)) {
		// it's ok if there isn't one
		return true;
	}

	// for enum and set types, get the values,
	// otherwise get length and scale
	if (enumorset) {

		// get values
		if (!parseValues(typenode,*newptr,newptr)) {
			return false;
		}

		// get right paren
		if (!rightParen(*newptr,newptr)) {
			debugPrintf("missing right paren\n");
			return false;
		}

	} else {

		// create node
		xmldomnode	*sizenode=newNode(typenode,_size);

		// get length
		if (!parseLength(sizenode,*newptr,newptr)) {
			debugPrintf("missing column length\n");
			return false;
		}

		// if theres a comma then we need to get the scale
		if (comma(*newptr,newptr)) {

			// get scale
			if (!parseScale(sizenode,*newptr,newptr)) {
				debugPrintf("missing scale\n");
				return false;
			}
		}

		// get right paren
		if (!rightParen(*newptr,newptr)) {
			debugPrintf("missing right paren\n");
			return false;
		}
	}

	// consume any spaces after the type
	space(*newptr,newptr);

	return true;
}

const char *sqlparser::_type="type";
const char *sqlparser::_size="size";

bool sqlparser::parseValues(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create new node
	xmldomnode	*valuesnode=newNode(currentnode,_values);

	*newptr=ptr;
	for (;;) {

		// get the value
		char	*value=getVerbatim(*newptr,newptr);
		if (!value) {
			debugPrintf("missing right paren\n");
			return false;
		}

		// create new node
		xmldomnode	*valuenode=
				newNode(valuesnode,_value);

		// set the value attribute
		setAttribute(valuenode,"value",value);

		// clean up
		delete[] value;

		// skip the next comma
		comma(*newptr,newptr);

		// if we hit a right parentheses then we're done, but we need
		// to stay on it, so we'll compare newptr directly rather than
		// using rightParen()
		if (**newptr==')') {
			return true;
		}
	}
}

const char *sqlparser::_values="values";
const char *sqlparser::_value="value";

bool sqlparser::parseLength(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	char	*length=getUntil(",)",ptr,newptr);
	newNode(currentnode,_length,length);
	delete[] length;
	return true;
}

const char *sqlparser::_length="length";

bool sqlparser::parseScale(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	char	*scale=getUntil(")",ptr,newptr);
	newNode(currentnode,_scale,scale);
	delete[] scale;
	return true;
}

const char *sqlparser::_scale="scale";

bool sqlparser::parseVerbatim(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// get the next block, verbatim
	char	*verbatim=getVerbatim(ptr,newptr);

	// if we got anything...
	bool	retval=charstring::length(verbatim);
	if (retval) {
		// create a new node and attribute
		newNode(currentnode,_verbatim,verbatim);
	} 

	// clean up
	delete[] verbatim;

	// return success or failure
	return retval;
}

const char *sqlparser::_verbatim="verbatim";

bool sqlparser::parseRemainderVerbatim(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	char	separator[2];
	separator[1]='\0';
	*newptr=ptr;
	do {
		if (inSet(**newptr,verbatimTerminators) && **newptr!=' ') {
			separator[0]=**newptr;
			newNode(currentnode,_verbatim,separator);
			(*newptr)++;
		} else {
			space(*newptr,newptr);
		}
	} while (parseVerbatim(currentnode,*newptr,newptr));
	return true;
}
