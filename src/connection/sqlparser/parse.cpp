// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>
#include <rudiments/character.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

bool sqlparser::whiteSpace(const char *ptr, const char **newptr) {
	while (*ptr && *ptr==' ') {
		ptr++;
	}
	*newptr=ptr;
	return true;
}

bool sqlparser::comparePart(const char *ptr, const char **newptr,
							const char *part) {
	debugFunction();

	// skip any whitespace
	whiteSpace(ptr,newptr);

	// get the part length
	uint64_t	length=charstring::length(part);

	// see if the next "length" bytes are equal to the part
	if (!charstring::compareIgnoringCase(*newptr,part,length)) {

		// if so then skip past the matching section
		*newptr=*newptr+length;

		// the part matched, return success
		return true;
	}

	// the part did not match, return failure
	*newptr=ptr;
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

	whiteSpace(ptr,newptr);

	// get the next block of whatever until we hit a space,
	// parenthesis, comma, operator, assignment operator or quotation
	return getUntil(" (,)~!^*-+=<>/%&|='\"",*newptr,newptr);
}

char *sqlparser::getClause(const char *ptr, const char *newptr) {
	const char	*start;
	whiteSpace(ptr,&start);
	return charstring::duplicate(start,newptr-start);
}

char *sqlparser::getUntil(const char *set,
				const char *ptr, const char **newptr) {
	debugFunction();

	whiteSpace(ptr,newptr);

	// find the next space, comma or right parentheses
	const char	*end=charstring::findFirstOfSetOrEnd(*newptr,set);

	// make a copy of the word we found
	char	*retval=charstring::duplicate(*newptr,end-*newptr);

	// set the return pointer
	*newptr=end;

	// return the word
	debugPrintf("getUntil: \"%s\"\n",retval);
	return retval;
}

static const char *verbatimterminators=" (,)~!^*-+=<>/%&|";

char *sqlparser::getVerbatim(const char *ptr, const char **newptr) {
	debugFunction();

	whiteSpace(ptr,newptr);

	// declare a buffer to store the data
	stringbuffer	verbatim;

	// set state flags
	bool	inquotes=false;
	bool	indoublequotes=false;

	// initialize the location
	const char	*chr=*newptr;

	// if we find a character in our termination set
	// then just return that by itself
	if (character::inSet(*chr,verbatimterminators)) {
		verbatim.append(*chr);
		*newptr=chr+1;
		debugPrintf("verbatim: \"%s\"\n",verbatim.getString());
		return verbatim.detachString();
	}

	// run through the string...
	for (;;) {

		// break on one of the termination characters
		// unless we're in quotes
		if (!*chr || (!inquotes && !indoublequotes &&
				character::inSet(*chr,verbatimterminators))) {
			break;
		}

		// if we encounter an escaped sequence then
		// append the entire escaped sequence and move on
		if ((useescapecharacters &&
			(inquotes || indoublequotes) &&
				*chr=='\\' && *(chr+1)) ||
			(inquotes && *chr=='\'' && *(chr+1)=='\'') ||
			(indoublequotes && *chr=='"' && *(chr+1)=='"')) {
			verbatim.append(*chr);
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
	return parseInternal(query,true) || parseInternal(query,false);
}

bool sqlparser::parseInternal(const char *query, bool useescapecharacters) {
	debugFunction();

	// set the useescapecharacters flag
	this->useescapecharacters=useescapecharacters;

	// initialze error status
	error=false;

	// create the tree
	delete tree;
	tree=new xmldom();
	tree->createRootNode();
	xmldomnode	*currentnode=tree->getRootNode();

	// parse the query
	char		*ptr=cleanQuery(query);
	const char	*newptr=ptr;
	if (!parseCreate(currentnode,ptr,&newptr) &&
		!parseDrop(currentnode,ptr,&newptr) &&
		!parseInsert(currentnode,ptr,&newptr) &&
		!parseUpdate(currentnode,ptr,&newptr) &&
		!parseDelete(currentnode,ptr,&newptr) &&
		!parseSelect(currentnode,ptr,&newptr) &&
		!parseSet(currentnode,ptr,&newptr) &&
		!parseLock(currentnode,ptr,&newptr)) {
		debugPrintf("unrecognized query\n");
		error=true;
	}
	delete[] ptr;

	printf("parse %susing escape characters %s\n\n",
				(useescapecharacters)?"":"without ",
				(error)?"failed":"succeeded");

	// return result
	return !error;
}

bool sqlparser::parseTableName(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	char	*tablename=getWord(ptr,newptr);
	splitDatabaseObjectName(currentnode,
				tablename,
				_table_name_database,
				_table_name_schema,
				_table_name_table);
	delete[] tablename;
	return true;
}

const char *sqlparser::_table_name_database="table_name_database";
const char *sqlparser::_table_name_schema="table_name_schema";
const char *sqlparser::_table_name_table="table_name_table";

void sqlparser::splitDatabaseObjectName(xmldomnode *currentnode,
						const char *name,
						const char *databasetag,
						const char *schematag,
						const char *objecttag) {
	debugFunction();

	// split the name
	char		**parts;
	uint64_t	count;
	charstring::split(name,".",true,&parts,&count);

	char	*db=NULL;
	char	*schema=NULL;
	char	*object=NULL;

	// combine initial parts into db name
	uint64_t	start=0;
	if (count>3) {
		stringbuffer	dbstr;
		for (start=0; start<count-2; start++) {
			if (start) {
				dbstr.append('.');
			}
			dbstr.append(parts[start]);
		}
		db=dbstr.detachString();
	} else if (count>2) {
		db=parts[start++];
	}

	// set schema, object names
	if (count>1) {
		schema=parts[start++];
	}
	if (count>0) {
		object=parts[start++];
	}

	// create nodes for each part
	if (db) {
		newNode(currentnode,databasetag,db);
	}
	if (schema) {
		newNode(currentnode,schematag,schema);
	}
	if (object) {
		newNode(currentnode,objecttag,object);
	}

	// clean up
	for (uint64_t i=0; i<count; i++) {
		delete[] parts[i];
	}
	delete[] parts;
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
			error=true;
			return false;
		}

	} else {

		// create node
		xmldomnode	*sizenode=newNode(typenode,_size);

		// get length
		if (!parseLength(sizenode,*newptr,newptr)) {
			debugPrintf("missing column length\n");
			error=true;
			return false;
		}

		// if theres a comma then we need to get the scale
		if (comma(*newptr,newptr)) {

			// get scale
			if (!parseScale(sizenode,*newptr,newptr)) {
				debugPrintf("missing scale\n");
				error=true;
				return false;
			}
		}

		// get right paren
		if (!rightParen(*newptr,newptr)) {
			debugPrintf("missing right paren\n");
			error=true;
			return false;
		}
	}

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
			error=true;
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
		// to stay on it, so we'll reset the pointer afterward if we
		// find one
		const char	*before=*newptr;
		if (rightParen(*newptr,newptr)) {
			*newptr=before;
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
	*newptr=ptr;
	do {
	} while (parseVerbatim(currentnode,*newptr,newptr));
	return true;
}
