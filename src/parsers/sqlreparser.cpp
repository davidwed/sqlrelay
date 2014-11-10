// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <rudiments/character.h>
#include <rudiments/charstring.h>
#include <rudiments/stringbuffer.h>
#include <sqlrelay/sqlreparser.h>
#include <debugprint.h>

sqlreparser::sqlreparser() : sqlrparser() {
	tree=NULL;
	foreigntree=false;
	error=false;
}

sqlreparser::~sqlreparser() {
	if (!foreigntree) {
		delete tree;
	}
}

void sqlreparser::useTree(xmldom *tree) {
	this->tree=tree;
	foreigntree=(tree!=NULL);
}

xmldom *sqlreparser::getTree() {
	return tree;
}

xmldom *sqlreparser::detachTree() {
	xmldom	*retval=tree;
	tree=NULL;
	return retval;
}

xmldomnode *sqlreparser::newNode(xmldomnode *parentnode, const char *type) {
	xmldomnode	*retval=new xmldomnode(tree,parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,type,NULL);
	parentnode->appendChild(retval);
	return retval;
}

xmldomnode *sqlreparser::newNode(xmldomnode *parentnode,
				const char *type, const char *value) {
	xmldomnode	*node=newNode(parentnode,type);
	setAttribute(node,_value,value);
	return node;
}

void sqlreparser::setAttribute(xmldomnode *node,
				const char *name, const char *value) {
	// FIXME: I shouldn't have to do this.
	// setAttribute should append it automatically
	if (node->getAttribute(name)!=node->getNullNode()) {
		node->setAttributeValue(name,value);
	} else {
		node->appendAttribute(name,value);
	}
}

char *sqlreparser::cleanQuery(const char *query) {

	// remove comments and convert all whitespace into spaces
	stringbuffer	cleanquery;
	const char	*ptr=query;
	for (;;) {

		// skip comments
		if (!charstring::compare(ptr,"-- ",3)) {
			while (*ptr && *ptr!='\n') {
				ptr++;
			}
			if (*ptr) {
				ptr++;
			}
		}

		// skip quoted strings
		if (*ptr=='\'') {
			do {
				cleanquery.append(*ptr);
				ptr++;
			} while (*ptr && *ptr!='\'');
			// NOTE: the trailing ' will be appended by code below
		}

		// skip double-quoted strings
		if (*ptr=='"') {
			do {
				cleanquery.append(*ptr);
				ptr++;
			} while (*ptr && *ptr!='"');
			// NOTE: the trailing " will be appended by code below
		}

		// convert whitespace into spaces
		if (character::isWhitespace(*ptr)) {
			cleanquery.append(' ');
		} else if (!*ptr) {
			break;
		} else {
			cleanquery.append(*ptr);
		}

		// move on to the next character
		ptr++;
	}
	debugPrintf("clean query:\n\"%s\"\n\n",cleanquery.getString());

	char	*retval=cleanquery.detachString();

	// trim any lingering leading/trailing whitespace
	charstring::bothTrim(retval);

	// return the clean query string
	debugPrintf("clean and fixed-up query:\n\"%s\"\n\n",retval);
	return retval;
}

bool sqlreparser::comma(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,",");
}

bool sqlreparser::equals(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"=");
}

bool sqlreparser::notEquals(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"!=",
		"<>",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlreparser::lessThan(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"<");
}

bool sqlreparser::greaterThan(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,">");
}

bool sqlreparser::lessThanOrEqualTo(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"<=");
}

bool sqlreparser::greaterThanOrEqualTo(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,">=");
}

bool sqlreparser::leftParen(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"(");
}

bool sqlreparser::rightParen(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,")");
}

bool sqlreparser::compliment(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"~");
}

bool sqlreparser::inverse(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"!");
}

bool sqlreparser::plus(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"+");
}

bool sqlreparser::minus(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"-");
}

bool sqlreparser::times(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"*");
}

bool sqlreparser::dividedBy(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"/");
}

bool sqlreparser::modulo(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"%");
}

bool sqlreparser::bitwiseAnd(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"&");
}

bool sqlreparser::bitwiseOr(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"|");
}

bool sqlreparser::bitwiseXor(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"^");
}

bool sqlreparser::logicalAnd(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"&&");
}

bool sqlreparser::logicalOr(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"||");
}

bool sqlreparser::endOfQuery(const char *ptr, const char **newptr) {
	debugFunction();
	whiteSpace(ptr,newptr);
	if (**newptr=='\0') {
		return true;
	}
	*newptr=ptr;
	return false;
}

bool sqlreparser::whiteSpace(const char *ptr, const char **newptr) {
	while (*ptr && *ptr==' ') {
		ptr++;
	}
	*newptr=ptr;
	return true;
}

bool sqlreparser::comparePart(const char *ptr, const char **newptr,
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

bool sqlreparser::comparePart(const char *ptr, const char **newptr,
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

char *sqlreparser::getWord(const char *ptr, const char **newptr) {
	debugFunction();

	whiteSpace(ptr,newptr);

	// get the next block of whatever until we hit a space,
	// parenthesis, comma, operator, assignment operator or quotation
	return getUntil(" (,)~!^*-+=<>/%&|='\"",*newptr,newptr);
}

char *sqlreparser::getClause(const char *ptr, const char *newptr) {
	const char	*start;
	whiteSpace(ptr,&start);
	return charstring::duplicate(start,newptr-start);
}

char *sqlreparser::getUntil(const char *set,
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

char *sqlreparser::getVerbatim(const char *ptr, const char **newptr) {
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

bool sqlreparser::parse(const char *query) {
	return parseInternal(query,true) || parseInternal(query,false);
}

bool sqlreparser::parseInternal(const char *query, bool useescapecharacters) {
	debugFunction();

	// set the useescapecharacters flag
	this->useescapecharacters=useescapecharacters;

	// initialze error status
	error=false;

	// create the tree
	if (!foreigntree) {
		delete tree;
		tree=new xmldom();
		tree->createRootNode();
	}
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
		!parseLock(currentnode,ptr,&newptr) &&
		!parseShow(currentnode,ptr,&newptr)) {
		debugPrintf("unrecognized query\n");
		error=true;
	}
	delete[] ptr;

	// return result
	return !error;
}

bool sqlreparser::parseTableName(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// save the starting point, in case this fails
	const char	*startptr=ptr;

	// get the table name
	char	*tablename=getWord(ptr,newptr);

	// Check to see if the table name started with a bind variable prefix.
	// If it does then it can't be a table name.  This is important because
	// select-into queries can select into either variables or tables,
	// this method gets called from parseSelectIntoTable and parseSelectInto
	char	c=tablename[0];
	if (c=='?' || c==':' || c=='@' || c=='$') {
		delete[] tablename;
		*newptr=startptr;
		return false;
	}

	// split the table name into db, schema and table components
	splitDatabaseObjectName(currentnode,
				tablename,
				_table_name_database,
				_table_name_schema,
				_table_name_table);

	// clean up
	delete[] tablename;
	return true;
}

const char *sqlreparser::_table_name_database="table_name_database";
const char *sqlreparser::_table_name_schema="table_name_schema";
const char *sqlreparser::_table_name_table="table_name_table";

void sqlreparser::splitDatabaseObjectName(xmldomnode *currentnode,
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

bool sqlreparser::parseName(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	char	*name=getWord(ptr,newptr);
	newNode(currentnode,_name,name);
	delete[] name;
	return true;
}

const char *sqlreparser::_name="name";

bool sqlreparser::parseType(xmldomnode *currentnode,
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

const char *sqlreparser::_type="type";
const char *sqlreparser::_size="size";

bool sqlreparser::parseValues(xmldomnode *currentnode,
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

const char *sqlreparser::_values="values";
const char *sqlreparser::_value="value";

bool sqlreparser::parseLength(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	char	*length=getUntil(",)",ptr,newptr);
	newNode(currentnode,_length,length);
	delete[] length;
	return true;
}

const char *sqlreparser::_length="length";

bool sqlreparser::parseScale(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	char	*scale=getUntil(")",ptr,newptr);
	newNode(currentnode,_scale,scale);
	delete[] scale;
	return true;
}

const char *sqlreparser::_scale="scale";

bool sqlreparser::parseVerbatim(xmldomnode *currentnode,
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

const char *sqlreparser::_verbatim="verbatim";

bool sqlreparser::parseRemainderVerbatim(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	*newptr=ptr;
	do {
	} while (parseVerbatim(currentnode,*newptr,newptr));
	return true;
}

bool sqlreparser::parseCreate(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create clause
	if (!createClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*createnode=newNode(currentnode,_create);

	// global (for tables)
	parseGlobal(createnode,*newptr,newptr);

	// temporary (for tables)
	parseTemporary(createnode,*newptr,newptr);

	// unique (for indices)
	parseUnique(createnode,*newptr,newptr);

	// fulltext (for indices)
	parseFulltext(createnode,*newptr,newptr);

	// spatial (for indices)
	parseSpatial(createnode,*newptr,newptr);

	// table, index, etc..
	if (parseCreateTable(createnode,*newptr,newptr) ||
		parseCreateIndex(createnode,*newptr,newptr) ||
		parseCreateSynonym(createnode,*newptr,newptr) ||
		parseCreatePackage(createnode,*newptr,newptr) ||
		parseCreateProcedure(createnode,*newptr,newptr) ||
		parseCreateFunction(createnode,*newptr,newptr)) {
		return true;
	}


	// for now we only support tables
	if (!error) {
		parseRemainderVerbatim(createnode,*newptr,newptr);
	}
	return true;
}

bool sqlreparser::createClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"create ");
}

const char *sqlreparser::_create="create";

bool sqlreparser::parseGlobal(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!globalClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_global);
	return true;
}

bool sqlreparser::globalClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"global ");
}

const char *sqlreparser::_global="global";

bool sqlreparser::parseTemporary(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!temporaryClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_temporary);
	return true;
}

bool sqlreparser::temporaryClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"temporary ",
		"temp ",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlreparser::_temporary="temporary";

bool sqlreparser::parseFulltext(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	xmldomnode	*newnode=NULL;
	return parseFulltext(currentnode,ptr,newptr,&newnode);
}

bool sqlreparser::parseFulltext(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					xmldomnode **newnode) {
	debugFunction();
	if (!fulltext(ptr,newptr)) {
		return false;
	}
	*newnode=newNode(currentnode,_fulltext);
	return true;
}

bool sqlreparser::fulltext(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"fulltext ");
}

const char *sqlreparser::_fulltext="fulltext";

bool sqlreparser::parseSpatial(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	xmldomnode	*newnode=NULL;
	return parseSpatial(currentnode,ptr,newptr,&newnode);
}

bool sqlreparser::parseSpatial(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					xmldomnode **newnode) {
	debugFunction();
	if (!spatial(ptr,newptr)) {
		return false;
	}
	*newnode=newNode(currentnode,_spatial);
	return true;
}

bool sqlreparser::spatial(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"spatial ");
}

const char *sqlreparser::_spatial="spatial";

bool sqlreparser::tableClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"table ");
}

bool sqlreparser::parseCreateTable(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// table
	if (!tableClause(ptr,newptr)) {
		return false;
	}

	// create new node
	xmldomnode	*tablenode=newNode(currentnode,_table);

	// if not exists
	parseIfNotExists(tablenode,*newptr,newptr);

	// table name
	parseTableName(tablenode,*newptr,newptr);

	// column and constrain definitions
	// (optional for create ... as select ...
	parseColumnAndConstraintDefinitions(tablenode,*newptr,newptr);

	// on commit (optional)
	parseOnCommit(tablenode,*newptr,newptr);

	// parse the remaining clauses
	for (;;) {

		// known clauses
		if (parseAs(tablenode,*newptr,newptr) ||
			parseWithNoLog(tablenode,*newptr,newptr)) {
			continue;
		}

		// if we find a select then that's the final clause
		if (parseSelect(tablenode,*newptr,newptr)) {
			continue;
		}

		// If we didn't encounter one of the known clauses
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand or until we hit the
		// end.
		if (!parseVerbatim(tablenode,*newptr,newptr)) {
			return true;
		}
	}
}

const char *sqlreparser::_table="table";

bool sqlreparser::parseIfNotExists(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!ifNotExistsClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_if_not_exists);
	return true;
}

bool sqlreparser::ifNotExistsClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"if not exists ");
}

const char *sqlreparser::_if_not_exists="if_not_exists";

bool sqlreparser::parseColumnAndConstraintDefinitions(
						xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// left paren
	if (!leftParen(ptr,newptr)) {
		return false;
	}

	// create new node
	xmldomnode	*columnsnode=newNode(currentnode,_columns);

	// column and constraint definitions
	for (;;) {

		// column definition or constraint
		const char	*start=*newptr;
		if (!parseConstraint(columnsnode,start,newptr) &&
			!parseColumnDefinition(columnsnode,start,newptr)) {
			return false;
		}

		// comma
		if (comma(*newptr,newptr)) {
			continue;
		}

		// right paren
		if (rightParen(*newptr,newptr)) {
			return true;
		}
	}
}

const char *sqlreparser::_columns="columns";
const char *sqlreparser::_column="column";

bool sqlreparser::parseColumnDefinition(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// create column definition node
	xmldomnode	*cnode=new xmldomnode(tree,
						currentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,
						_column,NULL);

	// column name
	if (!parseName(cnode,ptr,newptr)) {
		debugPrintf("missing column name\n");
		error=true;
		delete cnode;
		return false;
	}

	// column type
	if (!parseType(cnode,*newptr,newptr)) {
		debugPrintf("missing column type\n");
		error=true;
		delete cnode;
		return false;
	}

	// constraints
	parseConstraints(cnode,*newptr,newptr);

	// success
	currentnode->appendChild(cnode);
	return true;
}

bool sqlreparser::parseConstraints(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	xmldomnode	*constraintsnode=NULL;

	// there could be any number of constraints
	// and they could be in any order
	*newptr=ptr;
	for (;;) {

		// if we hit a comma or right parentheses then we're done,
		// but we need to stay on it, so we'll reset the pointer if
		// we find one
		const char	*before=*newptr;
		if (comma(*newptr,newptr)) {
			*newptr=before;
			break;
		}
		if (rightParen(*newptr,newptr)) {
			*newptr=before;
			break;
		}

		// create constraints node
		if (!constraintsnode) {
			constraintsnode=
				newNode(currentnode,_constraints);
		}

		// look for known constraints
		if (parseUnsigned(constraintsnode,*newptr,newptr) ||
			parseZeroFill(constraintsnode,*newptr,newptr) ||
			parseBinary(constraintsnode,*newptr,newptr) ||
			parseCharacterSet(constraintsnode,*newptr,newptr) ||
			parseCollate(constraintsnode,*newptr,newptr) ||
			parseNull(constraintsnode,*newptr,newptr) ||
			parseNotNull(constraintsnode,*newptr,newptr) ||
			parseDefault(constraintsnode,*newptr,newptr) ||
			parseAutoIncrement(constraintsnode,*newptr,newptr) ||
			parseUniqueKey(constraintsnode,*newptr,newptr) ||
			parsePrimaryKey(constraintsnode,*newptr,newptr) ||
			parseKey(constraintsnode,*newptr,newptr) ||
			parseComment(constraintsnode,*newptr,newptr) ||
			parseColumnFormat(constraintsnode,*newptr,newptr) ||
			parseReferenceDefinition(constraintsnode,
							*newptr,newptr)) {
			continue;
		}

		// If we didn't encounter one of the known constraint types,
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (!parseVerbatim(constraintsnode,*newptr,newptr)) {
			break;
		}
	}

	return true;
}

const char *sqlreparser::_constraints="constraints";

bool sqlreparser::parseUnsigned(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!unsignedClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_unsigned);
	return true;
}

bool sqlreparser::unsignedClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"unsigned");
}

const char *sqlreparser::_unsigned="unsigned";

bool sqlreparser::parseZeroFill(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!zeroFillClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_zerofill);
	return true;
}

bool sqlreparser::zeroFillClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"zerofill");
}

const char *sqlreparser::_zerofill="zerofill";

bool sqlreparser::parseBinary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!binaryClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_binary);
	return true;
}

bool sqlreparser::binaryClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"binary");
}

const char *sqlreparser::_binary="binary";

bool sqlreparser::parseCharacterSet(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// character set clause
	if (!characterSetClause(ptr,newptr)) {
		return false;
	}

	// character set itself
	char	*word=getWord(*newptr,newptr);
	newNode(currentnode,_character_set,word);
	delete[] word;
	return true;
}

bool sqlreparser::characterSetClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"character set ",
		"char set ",
		"charset ",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlreparser::_character_set="character_set";

bool sqlreparser::parseCollate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// collate clause
	if (!collateClause(ptr,newptr)) {
		return false;
	}

	// collation itself
	char	*word=getWord(*newptr,newptr);
	newNode(currentnode,_collate,word);
	delete[] word;
	return true;
}

bool sqlreparser::collateClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"collate");
}

const char *sqlreparser::_collate="collate";

bool sqlreparser::parseNull(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!nullClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_null);
	return true;
}

bool sqlreparser::nullClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"nullable",
		"null",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlreparser::_null="null";

bool sqlreparser::parseNotNull(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!notNullClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_not_null);
	return true;
}

bool sqlreparser::notNullClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"not nullable",
		"not null",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlreparser::_not_null="not_null";

bool sqlreparser::parseDefault(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// default value
	if (!defaultClause(ptr,newptr)) {
		return false;
	}

	// value itself
	char	*value=getVerbatim(*newptr,newptr);
	newNode(currentnode,_default,value);
	delete[] value;

	return true;
}

bool sqlreparser::defaultClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"default value ",
		"default ",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlreparser::_default="default";

bool sqlreparser::parseAutoIncrement(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!autoIncrementClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_auto_increment);
	return true;
}

bool sqlreparser::autoIncrementClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"auto_increment");
}

const char *sqlreparser::_auto_increment="auto_increment";

bool sqlreparser::parseIdentity(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!identityClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_identity);
	return true;
}

bool sqlreparser::identityClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"identity");
}

const char *sqlreparser::_identity="identity";

bool sqlreparser::parseUniqueKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!uniqueKeyClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_unique_key);
	return true;
}

bool sqlreparser::uniqueKeyClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"unique key",
		"unique",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlreparser::_unique_key="unique_key";

bool sqlreparser::parsePrimaryKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	xmldomnode	*newnode=NULL;
	return parsePrimaryKey(currentnode,ptr,newptr,&newnode);
}

bool sqlreparser::parsePrimaryKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						xmldomnode **newnode) {
	debugFunction();
	if (!primaryKeyClause(ptr,newptr)) {
		return false;
	}
	*newnode=newNode(currentnode,_primary_key);
	return true;
}

bool sqlreparser::primaryKeyClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"primary key");
}

const char *sqlreparser::_primary_key="primary_key";

bool sqlreparser::parseKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	xmldomnode	*newnode=NULL;
	return parseKey(currentnode,ptr,newptr,&newnode);
}

bool sqlreparser::parseKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						xmldomnode **newnode) {
	debugFunction();
	if (!keyClause(ptr,newptr)) {
		return false;
	}
	*newnode=newNode(currentnode,_key);
	return true;
}

bool sqlreparser::keyClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"key");
}

const char *sqlreparser::_key="key";

bool sqlreparser::parseComment(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// comment
	if (!commentClause(ptr,newptr)) {
		return false;
	}

	// comment itself
	char	*value=getVerbatim(*newptr,newptr);
	newNode(currentnode,_comment,value);
	delete[] value;

	return true;
}

bool sqlreparser::commentClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"comment ");
}

const char *sqlreparser::_comment="comment";

bool sqlreparser::parseColumnFormat(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// column format
	if (!columnFormatClause(ptr,newptr)) {
		return false;
	}

	// format itself
	char	*word=getWord(*newptr,newptr);
	newNode(currentnode,_column_format,word);
	delete[] word;
	return true;
}

bool sqlreparser::columnFormatClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"column_format ");
}

const char *sqlreparser::_column_format="column_format";

bool sqlreparser::parseReferenceDefinition(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	
	// references
	if (!referencesClause(ptr,newptr)) {
		return false;
	}

	// create references node
	xmldomnode	*referencesnode=newNode(currentnode,
						_references);

	// table name
	if (!parseTableName(referencesnode,*newptr,newptr)) {
		debugPrintf("missing table name\n");
		error=true;
		return false;
	}

	// column names
	if (!parseColumnNameList(referencesnode,*newptr,newptr)) {
		debugPrintf("missing column name list\n");
		error=true;
		return false;
	}

	// these are optional
	// try three times, we'll pick them all up if they're there
	// anything left must not be part of the reference definition
	// FIXME: this is dangerous, db's might have other options that don't
	// get picked up
	for (uint16_t tries=0; tries<3; tries++) {

		// match options
		parseMatch(referencesnode,*newptr,newptr);

		// on delete
		parseOnDelete(referencesnode,*newptr,newptr);

		// on update
		parseOnUpdate(referencesnode,*newptr,newptr);
	}

	// success
	return true;
}

bool sqlreparser::referencesClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"references ");
}

const char *sqlreparser::_references="references";

bool sqlreparser::parseColumnNameList(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// left paren
	if (!leftParen(ptr,newptr)) {
		return false;
	}

	// create new node
	xmldomnode	*columnsnode=newNode(currentnode,_columns);

	for (;;) {

		// get the column
		char	*column=getWord(*newptr,newptr);
		if (!charstring::length(column)) {
			debugPrintf("missing right paren\n");
			error=true;
			return false;
		}

		// create new node
		xmldomnode	*columnnode=
				newNode(columnsnode,_column);
		splitColumnName(columnnode,column);

		// clean up
		delete[] column;

		// skip the next comma
		if (comma(*newptr,newptr)) {
			continue;
		}

		// If we didn't find a comma, look for column length and an
		// asc/desc clause.  This method is used to parse the column
		// list for "create index" queries and they might have those
		// things.

		// length
		if (leftParen(*newptr,newptr)) {
			xmldomnode	*sizenode=newNode(columnnode,_size);
			parseLength(sizenode,*newptr,newptr);
			if (!rightParen(*newptr,newptr)) {
				debugPrintf("missing right paren\n");
				error=true;
				return false;
			}
		}

		// asc/desc
		parseAsc(columnnode,*newptr,newptr);
		parseDesc(columnnode,*newptr,newptr);

		// if we hit a right parentheses then we're done
		if (rightParen(*newptr,newptr)) {
			return true;
		}
	}
}

bool sqlreparser::parseMatch(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	
	// match
	if (!matchClause(ptr,newptr)) {
		return false;
	}

	// match option itself
	char	*word=getWord(*newptr,newptr);
	newNode(currentnode,_match,word);
	delete[] word;
	return true;
}

bool sqlreparser::matchClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"match ");
}

const char *sqlreparser::_match="match";

bool sqlreparser::parseOnDelete(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	
	// on delete
	if (!onDeleteClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*ondeletenode=
			newNode(currentnode,_on_delete);

	// reference option
	if (!parseReferenceOption(ondeletenode,*newptr,newptr)) {
		return false;
	}
	return true;
}

bool sqlreparser::onDeleteClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"on delete ");
}

const char *sqlreparser::_on_delete="on_delete";

bool sqlreparser::parseOnUpdate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	
	// on update
	if (!onUpdateClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*onupdatenode=
			newNode(currentnode,_on_update);

	// reference option
	if (!parseReferenceOption(onupdatenode,*newptr,newptr)) {
		return false;
	}
	return true;
}

bool sqlreparser::onUpdateClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"on update ");
}

const char *sqlreparser::_on_update="on_update";

bool sqlreparser::parseReferenceOption(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// is there a reference option clause at all
	if (!referenceOptionClause(ptr,newptr)) {
		return false;
	}

	// get the reference option
	char	*value=getClause(ptr,*newptr);

	// store it in the value attribute
	setAttribute(currentnode,_value,value);

	// clean up
	delete[] value;

	// success
	return true;
}

bool sqlreparser::referenceOptionClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"restrict",
		"cascade",
		"set null",
		"no action",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlreparser::parseOnCommit(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	
	// on commit
	if (!onCommitClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*oncommitnode=
			newNode(currentnode,_on_commit);

	// on commit option
	const char	*startptr=*newptr;
	if (!onCommitOptionClause(startptr,newptr)) {
		return false;
	}

	// get the option
	char	*value=getClause(startptr,*newptr);

	// store it in the value attribute
	setAttribute(oncommitnode,_value,value);

	// clean up
	delete[] value;

	// success
	return true;
}

bool sqlreparser::onCommitClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"on commit ");
}

const char *sqlreparser::_on_commit="on_commit";

bool sqlreparser::onCommitOptionClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"delete rows",
		"preserve rows",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlreparser::parseAs(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	if (!asClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_as);
	return true;
}

bool sqlreparser::asClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"as ");
}

const char *sqlreparser::_as="as";

bool sqlreparser::parseWithNoLog(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	const char	*start=ptr;
	if (comparePart(ptr,newptr,"with ") &&
		comparePart(*newptr,newptr,"no ") &&
		comparePart(*newptr,newptr,"log")) {
		newNode(currentnode,_with_no_log);
		return true;
	}
	*newptr=start;
	return false;
}

const char *sqlreparser::_with_no_log="with_no_log";

bool sqlreparser::parseCreateIndex(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// index
	if (!indexClause(ptr,newptr)) {
		return false;
	}

	// create new node
	xmldomnode	*indexnode=newNode(currentnode,_index);

	// index name
	if (!parseIndexName(indexnode,*newptr,newptr)) {
		debugPrintf("missing index name\n");
		error=true;
		return false;
	}

	// index type
	parseIndexType(indexnode,*newptr,newptr);

	// on
	if (!parseOnClause(indexnode,*newptr,newptr)) {
		debugPrintf("missing on clause\n");
		error=true;
		return false;
	}

	// table name
	if (!parseTableName(indexnode,*newptr,newptr)) {
		debugPrintf("missing table name\n");
		error=true;
		return false;
	}

	// column list
	if (!parseColumnNameList(indexnode,*newptr,newptr)) {
		debugPrintf("missing column name list\n");
		error=true;
		return false;
	}

	// index type
	parseIndexType(indexnode,*newptr,newptr);

	return true;
}

bool sqlreparser::indexClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"index ");
}

const char *sqlreparser::_index="index";

bool sqlreparser::parseIndexName(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	const char	*start=ptr;

	char	*indexname=getWord(ptr,newptr);

	// if the name was the word "using" then it was the beginning of an
	// index type rather than the name of the index itself
	bool	retval=true;
	if (!charstring::compareIgnoringCase(indexname,"using")) {
		*newptr=start;
		retval=false;
	} else {
		splitDatabaseObjectName(currentnode,
					indexname,
					_index_name_database,
					_index_name_schema,
					_index_name_index);
	}

	delete[] indexname;
	return retval;
}

const char *sqlreparser::_index_name_database="index_name_database";
const char *sqlreparser::_index_name_schema="index_name_schema";
const char *sqlreparser::_index_name_index="index_name_index";

bool sqlreparser::parseIndexType(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// using
	if (!usingClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*usingnode=newNode(currentnode,_using);

	// btree or hash
	if (parseBtree(usingnode,*newptr,newptr) ||
		parseHash(usingnode,*newptr,newptr)) {
		return true;
	}

	debugPrintf("missing index type\n");
	error=true;
	return false;
}

bool sqlreparser::parseBtree(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!btree(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_btree);
	return true;
}

bool sqlreparser::btree(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"btree");
}

const char *sqlreparser::_btree="btree";

bool sqlreparser::parseHash(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!hash(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_hash);
	return true;
}

bool sqlreparser::hash(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"hash");
}

const char *sqlreparser::_hash="hash";

bool sqlreparser::parseOnClause(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!onClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_on);
	return true;
}

bool sqlreparser::parseConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	return parseKeyConstraint(currentnode,ptr,newptr) ||
		parseIndexOrKeyConstraint(currentnode,ptr,newptr) ||
		parseFulltextOrSpatialConstraint(currentnode,ptr,newptr) ||
		parseCheckConstraint(currentnode,ptr,newptr);
}

bool sqlreparser::parseKeyConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// save the current position
	const char	*start=ptr;

	// create the node
	xmldomnode	*cnode=new xmldomnode(tree,
						currentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,
						_constraint,NULL);

	// get the constraint itself...
	*newptr=ptr;
	for (bool firsttry=true; ; firsttry=false) {

		// look for key clause
		if (parsePrimaryKeyConstraint(cnode,*newptr,newptr) ||
			parseUniqueConstraint(cnode,*newptr,newptr) ||
			parseForeignKeyConstraint(cnode,*newptr,newptr)) {
			currentnode->appendChild(cnode);
			return true;
		}

		if (error) {
			return false;
		}

		// look for a constraint clause, then try again for the key
		if (!firsttry || !parseConstraintClause(cnode,*newptr,newptr)) {
			*newptr=start;
			delete cnode;
			return false;
		}
	}
}

bool sqlreparser::parseConstraintClause(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// constraint
	if (!constraintClause(ptr,newptr)) {
		return false;
	}

	// save our spot
	const char	*start=*newptr;

	// get the name
	char	*name=getWord(*newptr,newptr);

	// we would like to add the name as the value of the constraint tag
	const char	*value=name;

	// ...but, if the name turns out to be a
	// key type then it wasn't a name at all
	if (!charstring::compareIgnoringCase(name,"primary") ||
		!charstring::compareIgnoringCase(name,"unique") ||
		!charstring::compareIgnoringCase(name,"foreign")) {

		// use an empty string instead, this will induce the writer
		// to still write out the constraint keyword, but without a
		// name following it
		value="";

		// go back to where we were before getting the name too
		*newptr=start;
	}

	// add whatever we decided on as the value of the constraint tag
	setAttribute(currentnode,_value,value);

	// clean up
	delete[] name;
	return true;
}

bool sqlreparser::constraintClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"constraint ");
}

const char *sqlreparser::_constraint="constraint";

bool sqlreparser::parsePrimaryKeyConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// primary key
	xmldomnode	*pkeynode=NULL;
	if (!parsePrimaryKey(currentnode,ptr,newptr,&pkeynode)) {
		return false;
	}

	// optional index type
	parseIndexType(pkeynode,*newptr,newptr);

	// column list
	if (!parseColumnNameList(pkeynode,*newptr,newptr)) {
		debugPrintf("missing column name list\n");
		error=true;
		return false;
	}

	// optional index option
	parseIndexOption(pkeynode,*newptr,newptr);
	return true;
}

bool sqlreparser::parseUniqueConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// unique (key)
	xmldomnode	*ukeynode=NULL;
	if (!parseUnique(currentnode,ptr,newptr,&ukeynode)) {
		return false;
	}

	// optional index and key
	if (!parseIndex(ukeynode,*newptr,newptr)) {
		parseKey(ukeynode,*newptr,newptr);
	}

	// optional index name
	parseIndexName(ukeynode,*newptr,newptr);

	// optional index type
	parseIndexType(ukeynode,*newptr,newptr);

	// column list
	if (!parseColumnNameList(ukeynode,*newptr,newptr)) {
		debugPrintf("missing column name list\n");
		error=true;
		return false;
	}

	// optional index option
	parseIndexOption(ukeynode,*newptr,newptr);
	return true;
}

bool sqlreparser::parseForeignKeyConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// foreign key
	xmldomnode	*fkeynode=NULL;
	if (!parseForeignKey(currentnode,ptr,newptr,&fkeynode)) {
		return false;
	}

	// optional index name
	parseIndexName(fkeynode,*newptr,newptr);

	// column list
	if (!parseColumnNameList(fkeynode,*newptr,newptr)) {
		debugPrintf("missing column name list\n");
		error=true;
		return false;
	}

	// reference definition
	parseReferenceDefinition(fkeynode,*newptr,newptr);
	return true;
}

bool sqlreparser::parseForeignKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						xmldomnode **newnode) {
	debugFunction();
	if (!foreignKeyClause(ptr,newptr)) {
		return false;
	}
	*newnode=newNode(currentnode,_foreign_key);
	return true;
}

bool sqlreparser::foreignKeyClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"foreign key");
}

const char *sqlreparser::_foreign_key="foreign_key";

bool sqlreparser::parseIndexOrKeyConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// index or key
	xmldomnode	*iknode=NULL;
	if (!parseIndex(currentnode,ptr,newptr,&iknode) &&
		!parseKey(currentnode,ptr,newptr,&iknode)) {
		return false;
	}

	// optional index name and type
	parseIndexName(iknode,*newptr,newptr);
	parseIndexType(iknode,*newptr,newptr);

	// column list
	if (!parseColumnNameList(iknode,*newptr,newptr)) {
		debugPrintf("missing column name list\n");
		error=true;
		return false;
	}

	// optional index option
	parseIndexOption(iknode,*newptr,newptr);
	return true;
}

bool sqlreparser::parseFulltextOrSpatialConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// fulltext or spatial
	xmldomnode	*fsnode=NULL;
	if (!parseFulltext(currentnode,ptr,newptr,&fsnode) &&
		!parseSpatial(currentnode,ptr,newptr,&fsnode)) {
		return false;
	}

	// optional index or key
	if (!parseIndex(fsnode,*newptr,newptr)) {
		parseKey(fsnode,*newptr,newptr);
	}

	// optional index name
	parseIndexName(fsnode,*newptr,newptr);

	// column list
	if (!parseColumnNameList(fsnode,*newptr,newptr)) {
		debugPrintf("missing column name list\n");
		error=true;
		return false;
	}

	// optional index option
	parseIndexOption(fsnode,*newptr,newptr);
	return true;
}

bool sqlreparser::parseIndex(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	xmldomnode	*newnode=NULL;
	return parseIndex(currentnode,ptr,newptr,&newnode);
}

bool sqlreparser::parseIndex(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						xmldomnode **newnode) {
	debugFunction();
	if (!indexClause(ptr,newptr)) {
		return false;
	}
	*newnode=newNode(currentnode,_index);
	return true;
}

bool sqlreparser::parseIndexOption(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	return parseKeyBlockSize(currentnode,ptr,newptr) ||
		parseIndexType(currentnode,ptr,newptr) ||
		parseWithParser(currentnode,ptr,newptr);
}

bool sqlreparser::parseKeyBlockSize(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	
	// key_block_size
	if (!keyBlockSize(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*kbsnode=newNode(currentnode,_key_block_size);

	// optional equals
	if (equals(*newptr,newptr)) {
		kbsnode->setAttributeValue("equals","true");
	}

	// numeric value
	bool	retval=true;
	char	*number=getVerbatim(*newptr,newptr);
	if (charstring::isNumber(number)) {
		kbsnode->setAttributeValue("value",number);
	} else {
		debugPrintf("missing key block size value\n");
		error=true;
		retval=false;
	}
	delete[] number;
	return retval;
}

bool sqlreparser::keyBlockSize(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"key_block_size");
}

const char *sqlreparser::_key_block_size="key_block_size";

bool sqlreparser::parseWithParser(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	
	// with parser
	if (!withParser(ptr,newptr)) {
		return false;
	}

	// get the parser name
	char	*name=getWord(*newptr,newptr);
	if (!name) {
		debugPrintf("missing parser name\n");
		error=true;
		return false;
	}

	// create the node
	newNode(currentnode,_with_parser,name);

	// clean up
	delete[] name;
	return true;
}

bool sqlreparser::withParser(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"with parser ");
}

const char *sqlreparser::_with_parser="with_parser";

bool sqlreparser::parseCheckConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// check
	if (!checkClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*cnode=newNode(currentnode,_check);

	// left paren
	if (!leftParen(*newptr,newptr)) {
		debugPrintf("missing left paren\n");
		error=true;
		return false;
	}

	// expression
	if (!parseExpression(cnode,*newptr,newptr)) {
		debugPrintf("missing expression\n");
		error=true;
		return false;
	}

	// right paren
	if (!rightParen(*newptr,newptr)) {
		debugPrintf("missing right paren\n");
		error=true;
		return false;
	}
	return true;
}

bool sqlreparser::checkClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"check ");
}

const char *sqlreparser::_check="check";

bool sqlreparser::parseCreateSynonym(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// synonym
	if (!synonymClause(ptr,newptr)) {
		return false;
	}

	// create new node
	xmldomnode	*synonymnode=newNode(currentnode,_synonym);

	// synonym name
	if (!parseDatabaseObjectName(synonymnode,*newptr,newptr)) {
		debugPrintf("missing object name\n");
		error=true;
		return false;
	}

	// for
	if (!parseFor(synonymnode,*newptr,newptr)) {
		debugPrintf("missing for\n");
		error=true;
		return false;
	}

	// original object name
	if (!parseDatabaseObjectName(synonymnode,*newptr,newptr)) {
		debugPrintf("missing object name\n");
		error=true;
		return false;
	}

	return true;
}

bool sqlreparser::synonymClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"synonym ");
}

const char *sqlreparser::_synonym="synonym";

bool sqlreparser::parseDatabaseObjectName(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	char	*objectname=getWord(ptr,newptr);
	splitDatabaseObjectName(currentnode,
				objectname,
				_object_name_database,
				_object_name_schema,
				_object_name_object);
	delete[] objectname;
	return true;
}

const char *sqlreparser::_object_name_database="object_name_database";
const char *sqlreparser::_object_name_schema="object_name_schema";
const char *sqlreparser::_object_name_object="object_name_object";

bool sqlreparser::parseFor(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!forClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_for);
	return true;
}

bool sqlreparser::forClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"for ");
}

const char *sqlreparser::_for="for";

bool sqlreparser::parseCreatePackage(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// we specifically don't want to parse these yet...

	// package
	if (packageClause(ptr,newptr)) {
		error=true;
	}
	return false;
}

bool sqlreparser::packageClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"package ");
}

const char *sqlreparser::_package="package";

bool sqlreparser::parseCreateProcedure(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// we specifically don't want to parse these yet...

	// procedure
	if (procedureClause(ptr,newptr)) {
		error=true;
	}
	return false;
}

bool sqlreparser::parseCreateFunction(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// we specifically don't want to parse these yet...

	// function
	if (functionClause(ptr,newptr)) {
		error=true;
	}
	return false;
}

bool sqlreparser::functionClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"function ");
}

bool sqlreparser::parseDelete(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a delete clause
	if (!deleteClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*deletenode=newNode(currentnode,_delete);

	// parse the delete clauses
	for (;;) {

		// look for the from clause
		if (parseDeleteFrom(deletenode,*newptr,newptr)) {
			break;
		}

		// If we didn't encounter one of the known clauses
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (!parseVerbatim(deletenode,*newptr,newptr)) {
			debugPrintf("missing from clause\n");
			error=true;
			return false;
		}
	}

	// table name
	// FIXME: in mysql, multiple tables may be specified
	if (!parseTableName(deletenode,*newptr,newptr)) {
		debugPrintf("missing table name\n");
		error=true;
		return false;
	}

	// parse the remaining clauses
	for (;;) {

		// look for known options
		if (parseUsing(deletenode,*newptr,newptr) ||
			parseWhere(deletenode,*newptr,newptr) ||
			parseOrderBy(deletenode,*newptr,newptr) ||
			parseLimit(deletenode,*newptr,newptr)) {
			continue;
		}

		// If we didn't encounter one of the known options
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (parseVerbatim(deletenode,*newptr,newptr)) {

			// if we find a comma, append that too
			if (comma(*newptr,newptr)) {
				newNode(deletenode,_verbatim,",");
			}

		} else {
			break;
		}
	}

	return true;
}

bool sqlreparser::deleteClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"delete ");
}

const char *sqlreparser::_delete="delete";

bool sqlreparser::parseDeleteFrom(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!deleteFromClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_delete_from);
	return true;
}

bool sqlreparser::deleteFromClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"from ");
}

const char *sqlreparser::_delete_from="delete_from";

bool sqlreparser::parseUsing(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!usingClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_using);
	return true;
}

bool sqlreparser::usingClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"using ");
}

const char *sqlreparser::_using="using";

bool sqlreparser::parseDrop(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a drop clause
	if (!dropClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*dropnode=newNode(currentnode,_drop);

	// temporary
	parseDropTemporary(dropnode,*newptr,newptr);

	// table, index, etc.
	if (parseDropTable(dropnode,*newptr,newptr) ||
		parseDropIndex(dropnode,*newptr,newptr)) {
		return true;
	}

	// for now we only support tables
	parseRemainderVerbatim(dropnode,*newptr,newptr);
	return true;
}

bool sqlreparser::dropClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"drop ");
}

const char *sqlreparser::_drop="drop";

bool sqlreparser::parseDropTemporary(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	if (!temporaryClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_drop_temporary);
	return true;
}

const char *sqlreparser::_drop_temporary="drop_temporary";

bool sqlreparser::parseDropTable(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// table
	if (!tableClause(ptr,newptr)) {
		return false;
	}

	// create new node
	xmldomnode	*tablenode=newNode(currentnode,_table);

	// if not exists
	parseIfExists(tablenode,*newptr,newptr);

	// table names
	if (!parseTableNameList(tablenode,*newptr,newptr)) {
		return false;
	}

	// restrict
	parseRestrict(tablenode,*newptr,newptr);

	// cascade
	parseCascade(tablenode,*newptr,newptr);

	// store anything trailing off the end verbatim
	parseRemainderVerbatim(tablenode,*newptr,newptr);

	return true;
}

bool sqlreparser::parseIfExists(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!ifExistsClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_if_exists);
	return true;
}

bool sqlreparser::ifExistsClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"if exists ");
}

const char *sqlreparser::_if_exists="if_exists";

bool sqlreparser::parseTableNameList(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create the node
	xmldomnode	*tablesnode=newNode(currentnode,_table_name_list);

	*newptr=ptr;
	for (;;) {

		// create the node
		xmldomnode	*tablenode=
				newNode(tablesnode,_table_name_list_item);

		// get the table name
		if (!parseTableName(tablenode,*newptr,newptr)) {
			return false;
		}

		// if there's a comma afterward then we have more
		// table names to get, otherwise we're done
		if (!comma(*newptr,newptr)) {
			return true;
		}
	}
}

const char *sqlreparser::_table_name_list="table_name_list";
const char *sqlreparser::_table_name_list_item="table_name_list_item";

bool sqlreparser::parseRestrict(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!restrictClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_restrict_clause);
	return true;
}

bool sqlreparser::restrictClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"restrict");
}

const char *sqlreparser::_restrict_clause="restrict";

bool sqlreparser::parseCascade(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// cascade
	if (!cascadeClause(ptr,newptr)) {
		return false;
	}

	// create node
	xmldomnode	*cascadenode=newNode(currentnode,_cascade);

	// look for constraints
	parseCascadeConstraintsClause(cascadenode,*newptr,newptr);
	return true;
}

bool sqlreparser::cascadeClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"cascade");
}

const char *sqlreparser::_cascade="cascade";

bool sqlreparser::parseCascadeConstraintsClause(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!cascadeConstraintsClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_cascade_constraints_clause);
	return true;
}

bool sqlreparser::cascadeConstraintsClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"constraints");
}

const char *sqlreparser::_cascade_constraints_clause="cascade_constraints_clause";

bool sqlreparser::parseDropIndex(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// index
	if (!indexClause(ptr,newptr)) {
		return false;
	}

	// create new node
	xmldomnode	*indexnode=newNode(currentnode,_index);

	// index name
	if (!parseIndexName(indexnode,*newptr,newptr)) {
		return false;
	}

	// on
	parseOnClause(indexnode,*newptr,newptr);

	// table name
	parseTableName(indexnode,*newptr,newptr);

	return true;
}

bool sqlreparser::parseInsert(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a insert clause
	if (!insertClause(ptr,newptr)) {
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
		if (!parseVerbatim(insertnode,*newptr,newptr)) {
			break;
		}
	}

	// "values" or "value" clause
	if (!parseInsertValues(insertnode,*newptr,newptr) &&
		!parseInsertValue(insertnode,*newptr,newptr) &&
		!parseUpdateSet(insertnode,*newptr,newptr,false) &&
		!parseSelect(insertnode,*newptr,newptr)) {
		debugPrintf("missing value, values, set or select clause\n");
		error=true;
		return false;
	}

	parseRemainderVerbatim(insertnode,*newptr,newptr);
	return true;
}

bool sqlreparser::insertClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"insert ");
}

const char *sqlreparser::_insert="insert";

bool sqlreparser::parseInsertInto(xmldomnode *currentnode,
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
	if (!parseTableName(intonode,*newptr,newptr)) {
		debugPrintf("missing table name\n");
		error=true;
		return false;
	}

	// columns (optional)
	parseColumnNameList(intonode,*newptr,newptr);

	return true;
}

bool sqlreparser::insertIntoClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"into ");
}

const char *sqlreparser::_insert_into="insert_into";

bool sqlreparser::parseInsertValues(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// values
	if (!insertValuesClause(ptr,newptr)) {
		return false;
	}
	
	// create the node
	xmldomnode	*valuesnode=newNode(currentnode,_insert_values_clause);

	// the values themselves
	return parseInsertValuesList(valuesnode,*newptr,newptr);
}

bool sqlreparser::insertValuesClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"values ");
}

const char *sqlreparser::_insert_values_clause="insert_values_clause";

bool sqlreparser::parseInsertValue(xmldomnode *currentnode,
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

bool sqlreparser::insertValueClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"value ");
}

const char *sqlreparser::_insert_value_clause="insert_value_clause";

bool sqlreparser::parseInsertValuesList(xmldomnode *currentnode,
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

		// get the value to insert
		xmldomnode	*valuenode=newNode(currentnode,_insert_value);
		if (!parseExpression(valuenode,*newptr,newptr)) {
			return false;
		}

		// skip the comma, if there was one
		comma(*newptr,newptr);
	}
}

const char *sqlreparser::_insert_value="insert_value";

bool sqlreparser::parseSelect(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// right away, look for subselects
	if (parseSubSelects(currentnode,ptr,newptr)) {
		return true;
	}

	bool	foundunion=false;

	for (;;) {

		// look for a select clause
		if (!selectClause(*newptr,newptr)) {
			return false;
		}

		// create the node
		xmldomnode	*selectnode=newNode(currentnode,_select);

		// parse the select options
		do {} while (parseAll(selectnode,*newptr,newptr) ||
			parseUnique(selectnode,*newptr,newptr) ||
			parseDistinct(selectnode,*newptr,newptr) ||
			parseDistinctRow(selectnode,*newptr,newptr) ||
			parseHighPriority(selectnode,*newptr,newptr) ||
			parseStraightJoinSelectOption(selectnode,
							*newptr,newptr) ||
			parseSqlSmallResult(selectnode,*newptr,newptr) ||
			parseSqlBigResult(selectnode,*newptr,newptr) ||
			parseSqlBufferResult(selectnode,*newptr,newptr) ||
			parseSqlCache(selectnode,*newptr,newptr) ||
			parseSqlNoCache(selectnode,*newptr,newptr) ||
			parseSqlCalcFoundRows(selectnode,*newptr,newptr));

		// create a node
		xmldomnode	*selectexprsnode=
					newNode(selectnode,_select_expressions);

		// parse the expressions
		bool	first=true;
		for (;;) {

			// look for from and into clauses
			// (do this first so the words "from" and "into"
			// don't get mistaken for a term in an expression
			const char	*beforefrom=*newptr;
			if (fromClause(*newptr,newptr) ||
				insertIntoClause(*newptr,newptr)) {
				*newptr=beforefrom;
				if (first) {
					debugPrintf("no select expressions\n");
					error=true;
					return false;
				}
				break;
			}

			// there should be a comma after each expression
			if (!first) {

				// well, actually...
				// check for the end of the query, some db's
				// allow selects without a from clause
				if (!**newptr) {
					return true;
				}

				// check for the comma
				if (!comma(*newptr,newptr)) {
					debugPrintf("missing comma\n");
					error=true;
					return false;
				}
			}

			// create a node
			xmldomnode	*selectexprnode=
					newNode(selectexprsnode,
						_select_expression);

			// look for expressions and subqueries
			if (parseSubSelects(selectexprnode,
						*newptr,newptr) ||
				parseExpression(selectexprnode,
						*newptr,newptr)) {

				// alias
				parseSelectExpressionAlias(selectexprnode,
								*newptr,newptr);
				first=false;
				continue;
			}

			// if we got here then there was something weird where
			// we'd expect a from clause 
			debugPrintf("something weird instead of from clause\n");
			error=true;
			return false;
		}

		// parse the select clauses
		uint16_t	parenlevel=0;
		foundunion=false;
		for (;;) {

			// look for known options
			if (parseFrom(selectnode,*newptr,newptr) ||
				parseWhere(selectnode,*newptr,newptr) ||
				parseGroupBy(selectnode,*newptr,newptr) ||
				parseHaving(selectnode,*newptr,newptr) ||
				parseOrderBy(selectnode,*newptr,newptr) ||
				parseLimit(selectnode,*newptr,newptr) ||
				parseProcedure(selectnode,*newptr,newptr) ||
				parseSelectIntoTable(
						selectnode,*newptr,newptr) ||
				parseSelectIntoVariables(
						selectnode,*newptr,newptr) ||
				parseForUpdate(selectnode,*newptr,newptr) ||
				parseNoWait(selectnode,*newptr,newptr)) {
				continue;
			}

			// count parens
			const char	*before=*newptr;
			if (leftParen(*newptr,newptr)) {
				*newptr=before;
				parenlevel++;
			} else if (rightParen(*newptr,newptr)) {
				*newptr=before;
				// If we hit a right paren that we can't
				// account for then this must have been a
				// nested subselect.  Bail.
				if (parenlevel==0) {
					break;
				}
				parenlevel--;
			}

			// look for a union
			if (parseUnion(currentnode,*newptr,newptr)) {
				foundunion=true;
				break;
			}

			// If we didn't encounter one of the known options then
			// there must be something in there that we don't
			// understand.  It needs to be copied verbatim until we
			// run into something that we do understand.
			if (!parseVerbatim(selectnode,*newptr,newptr)) {
				break;
			}
		}

		// if we found a union then keep going
		if (foundunion) {
			continue;
		}

		// otherwise we ought to be done
		return true;
	}
}

bool sqlreparser::selectClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"select ");
}

const char *sqlreparser::_select="select";
const char *sqlreparser::_select_expressions="select_expressions";
const char *sqlreparser::_select_expression="select_expression";

bool sqlreparser::parseSelectExpressionAlias(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	return parseAlias(currentnode,ptr,newptr,false);
}

bool sqlreparser::parseAlias(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						bool subselect) {
	debugFunction();

	const char	*reset=ptr;
	bool		retval=true;
	char		*as=NULL;

	// get the next word
	char	*word=getVerbatim(ptr,newptr);

	// if it's "as" then get the word after that
	if (!charstring::compareIgnoringCase(word,sqlreparser::_as)) {
		as=word;
		reset=*newptr;
		word=getVerbatim(*newptr,newptr);
	}

	// see if the word was something that would come after a select
	// expression that can't be an alias
	if (!charstring::length(word) ||
		!charstring::compareIgnoringCase(word,",") ||
		!charstring::compareIgnoringCase(word,")") ||
		// from clause
		!charstring::compareIgnoringCase(word,sqlreparser::_from) ||
		// for clause
		!charstring::compareIgnoringCase(word,"for") ||
		// order by clause
		!charstring::compareIgnoringCase(word,"order") ||
		// group by clause
		!charstring::compareIgnoringCase(word,"group") ||
		// into clause
		!charstring::compareIgnoringCase(word,"into") ||
		(subselect &&
		// where clause
		(!charstring::compareIgnoringCase(word,sqlreparser::_where) ||
		// join clauses
		!charstring::compareIgnoringCase(word,sqlreparser::_inner) ||
		!charstring::compareIgnoringCase(word,sqlreparser::_cross) ||
		!charstring::compareIgnoringCase(
					word,sqlreparser::_straight_join) ||
		!charstring::compareIgnoringCase(word,sqlreparser::_left) ||
		!charstring::compareIgnoringCase(word,sqlreparser::_right) ||
		!charstring::compareIgnoringCase(word,sqlreparser::_natural) ||
		!charstring::compareIgnoringCase(word,sqlreparser::_on) ||
		!charstring::compareIgnoringCase(word,sqlreparser::_using) ||
		// union clause
		!charstring::compareIgnoringCase(word,sqlreparser::_union)))) {

		// if so...
		if (charstring::length(as)) {
			// if there was an "as" then it must be the alias
			// store it and reset to before this last word
			newNode(currentnode,_alias,as);
			*newptr=reset;
		} else {
			// there was no as or alias, reset and bail
			*newptr=reset;
			retval=false;
		}

	} else {

		// if not, append the alias
		if (charstring::length(as)) {
			currentnode=newNode(currentnode,_as);
		}
		newNode(currentnode,_alias,word);
	}

	// clean up
	delete[] word;
	delete[] as;

	return retval;
}

bool sqlreparser::parseSubSelects(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	*newptr=ptr;
	for (;;) {

		// left paren
		if (!leftParen(*newptr,newptr)) {
			return false;
		}

		// create the node
		xmldomnode	*subselectnode=newNode(currentnode,_sub_select);

		// parse the subselect
		if (!parseSelect(subselectnode,*newptr,newptr)) {
			// apparently this isn't a select at all, but some
			// other grouping, reset and bail
			*newptr=ptr;
			currentnode->deleteChild(subselectnode);
			return false;
		}

		// right paren
		if (!rightParen(*newptr,newptr)) {
			debugPrintf("missing right paren after subselect\n");
			error=true;
			return false;
		}

		// alias
		parseSubSelectAlias(currentnode,*newptr,newptr);

		// unions
		if (parseUnion(currentnode,*newptr,newptr)) {
			continue;
		}

		// FIXME: mysql index hints...
		return true;
	}
}

const char *sqlreparser::_sub_select="sub_select";

bool sqlreparser::parseSubSelectAlias(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	return parseAlias(currentnode,ptr,newptr,true);
}

const char *sqlreparser::_alias="alias";

bool sqlreparser::parseUnion(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// union
	if (!unionClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*unionnode=newNode(currentnode,_union);

	// all
	parseAll(unionnode,*newptr,newptr);

	// distinct
	parseDistinct(unionnode,*newptr,newptr);
	return true;
}

bool sqlreparser::unionClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"union ");
}

const char *sqlreparser::_union="union";

bool sqlreparser::parseAll(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!allClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_all);
	return true;
}

bool sqlreparser::allClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"all ");
}

const char *sqlreparser::_all="all";

bool sqlreparser::parseUnique(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	xmldomnode	*newnode=NULL;
	return parseUnique(currentnode,ptr,newptr,&newnode);
}

bool sqlreparser::parseUnique(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					xmldomnode **newnode) {
	debugFunction();
	if (!uniqueClause(ptr,newptr)) {
		return false;
	}
	*newnode=newNode(currentnode,_unique);
	return true;
}

bool sqlreparser::uniqueClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"unique ");
}

const char *sqlreparser::_unique="unique";

bool sqlreparser::parseDistinct(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!distinctClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_distinct);
	return true;
}

bool sqlreparser::distinctClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"distinct ");
}

const char *sqlreparser::_distinct="distinct";

bool sqlreparser::parseDistinctRow(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!distinctRowClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_distinct_row);
	return true;
}

bool sqlreparser::distinctRowClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"distinctrow ");
}

const char *sqlreparser::_distinct_row="distinct_row";

bool sqlreparser::parseHighPriority(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!highPriorityClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_high_priority);
	return true;
}

bool sqlreparser::highPriorityClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"high_priority ");
}

const char *sqlreparser::_high_priority="high_priority";

bool sqlreparser::parseStraightJoinSelectOption(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!straightJoinClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_straight_join_select_option);
	return true;
}

bool sqlreparser::straightJoinClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"straight_join ");
}

const char *sqlreparser::_straight_join_select_option=
					"straight_join_select_option";

bool sqlreparser::parseSqlSmallResult(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!sqlSmallResultClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_sql_small_result);
	return true;
}

bool sqlreparser::sqlSmallResultClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"sql_small_result ");
}

const char *sqlreparser::_sql_small_result="sql_small_result";

bool sqlreparser::parseSqlBigResult(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!sqlBigResultClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_sql_big_result);
	return true;
}

bool sqlreparser::sqlBigResultClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"sql_big_result ");
}

const char *sqlreparser::_sql_big_result="sql_big_result";

bool sqlreparser::parseSqlBufferResult(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!sqlBufferResultClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_sql_buffer_result);
	return true;
}

bool sqlreparser::sqlBufferResultClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"sql_buffer_result ");
}

const char *sqlreparser::_sql_buffer_result="sql_buffer_result";

bool sqlreparser::parseSqlCache(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!sqlCacheClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_sql_cache);
	return true;
}

bool sqlreparser::sqlCacheClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"sql_cache ");
}

const char *sqlreparser::_sql_cache="sql_cache";

bool sqlreparser::parseSqlNoCache(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!sqlNoCacheClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_sql_no_cache);
	return true;
}

bool sqlreparser::sqlNoCacheClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"sql_no_cache ");
}

const char *sqlreparser::_sql_no_cache="sql_no_cache";

bool sqlreparser::parseSqlCalcFoundRows(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!sqlCalcFoundRowsClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_sql_calc_found_rows);
	return true;
}

bool sqlreparser::sqlCalcFoundRowsClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"sql_calc_found_rows ");
}

const char *sqlreparser::_sql_calc_found_rows="sql_calc_found_rows";

bool sqlreparser::parseGroupBy(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for group by
	if (!groupByClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*groupbynode=newNode(currentnode,_group_by);

	for (;;) {
		
		// create the node
		xmldomnode	*groupbyitemnode=
				newNode(groupbynode,_group_by_item);

		if (!parseExpression(groupbyitemnode,*newptr,newptr)) {	
			debugPrintf("missing group by expression\n");
			error=true;
			return false;
		}

		// asc
		parseAsc(groupbyitemnode,*newptr,newptr);

		// desc
		parseDesc(groupbyitemnode,*newptr,newptr);

		// comma
		if (!comma(*newptr,newptr)) {
			break;
		}
	}

	// with rollup (optional)
	parseWithRollup(groupbynode,*newptr,newptr);

	return true;
}

bool sqlreparser::groupByClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"group by ");
}

const char *sqlreparser::_group_by="group_by";
const char *sqlreparser::_group_by_item="group_by_item";

bool sqlreparser::parseWithRollup(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!withRollupClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_with_rollup);
	return true;
}

bool sqlreparser::withRollupClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"with rollup");
}

const char *sqlreparser::_with_rollup="with_rollup";

bool sqlreparser::parseOrderBy(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for order by
	if (!orderByClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*orderbynode=newNode(currentnode,_order_by);

	for (;;) {
		
		// create the node
		xmldomnode	*orderbyitemnode=
				newNode(orderbynode,_order_by_item);

		if (!parseExpression(orderbyitemnode,*newptr,newptr)) {	
			debugPrintf("missing order by expression\n");
			error=true;
			return false;
		}

		// asc
		parseAsc(orderbyitemnode,*newptr,newptr);

		// desc
		parseDesc(orderbyitemnode,*newptr,newptr);

		// comma
		if (!comma(*newptr,newptr)) {
			break;
		}
	}

	return true;
}

bool sqlreparser::orderByClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"order by ");
}

const char *sqlreparser::_order_by="order_by";
const char *sqlreparser::_order_by_item="order_by_item";

bool sqlreparser::parseAsc(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!asc(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_asc);
	return true;
}

bool sqlreparser::asc(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"asc");
}

const char *sqlreparser::_asc="asc";

bool sqlreparser::parseDesc(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!desc(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_desc);
	return true;
}

bool sqlreparser::desc(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"desc");
}

const char *sqlreparser::_desc="desc";

bool sqlreparser::parseLimit(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!limitClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_limit);
	return true;
}

bool sqlreparser::limitClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"limit ");
}

const char *sqlreparser::_limit="limit";

bool sqlreparser::parseProcedure(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!procedureClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_procedure);
	return true;
}

bool sqlreparser::procedureClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"procedure ");
}

const char *sqlreparser::_procedure="procedure";

bool sqlreparser::parseSelectIntoTable(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// into
	if (!selectIntoClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*intonode=newNode(currentnode,_select_into);

	// FIXME: Most db's don't require the temp qualifier here and it's
	// not impossible someone could name their temp table "temp".
	// Currently this code doesn't handle that.

	// temp (optional)
	parseTemporary(intonode,*newptr,newptr);

	// table name
	if (!parseTableName(intonode,*newptr,newptr)) {
		debugPrintf("missing table name\n");
		error=true;
		return false;
	}

	// columns (optional)
	parseColumnNameList(intonode,*newptr,newptr);

	return true;
}

bool sqlreparser::parseSelectIntoVariables(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!selectIntoClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_select_into);
	return true;
}

bool sqlreparser::selectIntoClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"into ");
}

const char *sqlreparser::_select_into="select_into";

bool sqlreparser::parseForUpdate(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!forUpdateClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_for_update);
	return true;
}

bool sqlreparser::forUpdateClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"for update");
}

const char *sqlreparser::_for_update="for_update";

bool sqlreparser::parseUpdate(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a update clause
	if (!updateClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*updatenode=newNode(currentnode,_update);

	// find the table name
	const char	*endofupdateptr=*newptr;
	const char	*tablenameptr=*newptr;
	for (;;) {

		// if we hit the end of the string then we've got a problem
		if (!**newptr) {
			debugPrintf("missing set clause\n");
			error=true;
			return false;
		}

		// look for the set clause
		const char	*startptr=*newptr;
		if (updateSetClause(*newptr,newptr)) {

			// find the space before the word before it
			*newptr=startptr-2;
			while (!character::isWhitespace(**newptr) &&
							*newptr!=ptr) {
				*newptr=*newptr-1;
			}

			// if we hit the beginning of the string then we've got
			// a problem
			if (*newptr==ptr) {
				debugPrintf("missing table name\n");
				error=true;
				return false;
			}

			// skip any whitespace
			whiteSpace(*newptr,newptr);

			tablenameptr=*newptr;
			break;
		}

		// move on
		*newptr=*newptr+1;
	}

	// parse everything between the update clause and the table name
	*newptr=endofupdateptr;

	// parse the update clauses
	for (;;) {

		// if we hit the table name then we're done
		whiteSpace(*newptr,newptr);
		if (*newptr==tablenameptr) {
			break;
		}

		// parse known clauses...
		// FIXME: known clauses?

		// If we didn't encounter one of the known clauses
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (!parseVerbatim(updatenode,*newptr,newptr)) {
			break;
		}
	}

	// table name
	if (!parseTableName(updatenode,*newptr,newptr)) {
		debugPrintf("missing table name\n");
		error=true;
		return false;
	}

	// set clause
	if (!parseUpdateSet(updatenode,*newptr,newptr,true)) {
		return false;
	}

	// parse the remaining clauses
	for (;;) {

		// look for known clauses
		if (parseWhere(updatenode,*newptr,newptr) ||
			parseOrderBy(updatenode,*newptr,newptr) ||
			parseLimit(updatenode,*newptr,newptr)) {
			continue;
		}

		// If we didn't encounter one of the known options
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (parseVerbatim(updatenode,*newptr,newptr)) {

			// if we find a comma, append that too
			if (comma(*newptr,newptr)) {
				newNode(updatenode,_verbatim,",");
			}

		} else {
			break;
		}
	}

	return true;
}

bool sqlreparser::updateClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"update ");
}

const char *sqlreparser::_update="update";

bool sqlreparser::parseUpdateSet(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					bool required) {
	debugFunction();

	// set clause
	if (!updateSetClause(ptr,newptr)) {
		if (required) {
			debugPrintf("missing set\n");
			error=true;
		}
		return false;
	}

	// create the node
	xmldomnode	*setnode=newNode(currentnode,_update_set);

	// parse the assignments
	for (;;) {

		// if we hit the end of the string then we're done
		if (!**newptr) {
			return true;
		}

		// when we find the where clause, we're done
		const char	*startptr=*newptr;
		if (whereClause(*newptr,newptr)) {
			// reset the pointer
			*newptr=startptr;
			return true;
		}

		// create the node
		xmldomnode	*assignmentnode=newNode(setnode,_assignment);

		// get the column to assign to
		xmldomnode	*colrefnode=
				newNode(assignmentnode,_column_reference);
		char	*column=getWord(*newptr,newptr);
		splitColumnName(colrefnode,column);
		delete[] column;

		// skip past the equals sign
		if (!equals(*newptr,newptr)) {
			debugPrintf("missing equals sign\n");
			error=true;
			return false;
		}
		newNode(assignmentnode,_equals);

		// get the value assigned to the column
		if (!parseExpression(assignmentnode,*newptr,newptr)) {
			error=true;
			return false;
		}

		// skip the comma, if there was one
		comma(*newptr,newptr);
	}
}

bool sqlreparser::updateSetClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"set ");
}

const char *sqlreparser::_update_set="update_set";
const char *sqlreparser::_assignment="assignment";
const char *sqlreparser::_equals="equals";

bool sqlreparser::parseSet(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a set clause
	if (!setClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*setnode=newNode(currentnode,_set);

	// global
	parseSetGlobal(setnode,*newptr,newptr);

	// session
	parseSetSession(setnode,*newptr,newptr);

	// look for known options
	if (parseTransaction(setnode,*newptr,newptr)) {
		return true;
	}

	// for now we only support transactions
	parseRemainderVerbatim(setnode,*newptr,newptr);
	return true;
}

bool sqlreparser::setClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"set ");
}

const char *sqlreparser::_set="set";

bool sqlreparser::parseSetGlobal(xmldomnode *currentnode,
				const char *ptr,
				const char **newptr) {
	if (!setGlobalClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_set_global);
	return true;
}

bool sqlreparser::setGlobalClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"global ");
}

const char *sqlreparser::_set_global="set_global";

bool sqlreparser::parseSetSession(xmldomnode *currentnode,
				const char *ptr,
				const char **newptr) {
	if (!setSessionClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_set_session);
	return true;
}

bool sqlreparser::setSessionClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"session ");
}

const char *sqlreparser::_set_session="set_session";

bool sqlreparser::parseTransaction(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// transaction
	if (!transactionClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*txnode=newNode(currentnode,_transaction);

	// parse the remaining clauses
	for (;;) {

		// known clauses
		if (parseIsolationLevel(txnode,*newptr,newptr)) {
			continue;
		}

		// If we didn't encounter one of the known clauses
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand or until we hit the
		// end.
		if (!parseVerbatim(txnode,*newptr,newptr)) {
			return true;
		}
	}
	return true;
}

bool sqlreparser::transactionClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"transaction ");
}

const char *sqlreparser::_transaction="transaction";

bool sqlreparser::parseIsolationLevel(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// isolation level
	if (!isolationLevelClause(ptr,newptr)) {
		return false;
	}
	
	// create the node
	xmldomnode	*isonode=newNode(currentnode,_isolation_level);

	// value
	const char	*startptr=*newptr;
	if (isolationLevelOptionClause(startptr,newptr)) {
		char	*value=getClause(startptr,*newptr);
		setAttribute(isonode,_value,value);
		delete[] value;
		return true;
	}

	parseRemainderVerbatim(isonode,*newptr,newptr);
	return true;
}

bool sqlreparser::isolationLevelClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"isolation level ");
}

const char *sqlreparser::_isolation_level="isolation_level";

bool sqlreparser::isolationLevelOptionClause(const char *ptr,
						const char **newptr) {
	debugFunction();
	const char *parts[]={
		// standard SQL
		"read uncommitted",
		"read committed",
		"repeatable read",
		"serializable",
		// DB2
		"cursor stability","cs",
		"repeatable read","rr",
		"read stability","rs",
		"uncomitted read","ur"
		// Sybase
		"0", // read uncommited
		"1", // read committed
		"2", // repeatable read
		"3", // serializable
		// Firebird
		"SNAPSHOT",
		"SNAPSHOT TABLE STABILITY",
		"READ COMMITTED NO RECORD_VERSION",
		"READ COMMITTED RECORD VERSION",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlreparser::parseFrom(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for from
	if (!fromClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*fromnode=newNode(currentnode,_from);

	return parseTableReferences(fromnode,*newptr,newptr);
}

bool sqlreparser::fromClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"from ");
}

const char *sqlreparser::_from="from";

bool sqlreparser::parseTableReferences(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// FIXME: can there be parens around these sometimes?

	xmldomnode	*tablerefsnode=newNode(currentnode,_table_references);

	// parse references, subselects and joins
	bool	first=true;
	*newptr=ptr;
	for (;;) {

		// if it's the first time through,
		// or if we find a comma, the next thing
		// will be a reference or subselect
		if (first || comma(*newptr,newptr)) {

			first=false;

			// parse references and subselects
			if (!parseSubSelects(tablerefsnode,
							*newptr,newptr) &&
				!parseTableReference(tablerefsnode,
							*newptr,newptr)) {
				debugPrintf("missing table reference "
						"in select expr\n");
				error=true;
				return false;
			}

			continue;
		}

		// if we find a join, process it, then loop
		// back and look for another reference
		if (parseJoin(tablerefsnode,*newptr,newptr)) {
			continue;
		}

		// otherwise, we're done
		break;
	}

	return true;
}

const char *sqlreparser::_table_references="table_references";

bool sqlreparser::parseTableReference(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// get the name
	char	*word=getWord(ptr,newptr);
	if (!charstring::length(word)) {
		delete[] word;
		return false;
	}

	// create the nodes
	xmldomnode	*tablerefnode=newNode(currentnode,_table_reference);
	splitDatabaseObjectName(tablerefnode,
				word,
				_table_name_database,
				_table_name_schema,
				_table_name_table);

	// alias
	parseSubSelectAlias(tablerefnode,*newptr,newptr);

	// FIXME: mysql index hints...

	delete[] word;
	return true;
}

const char *sqlreparser::_table_reference="table_reference";

bool sqlreparser::parseJoin(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// preceeding clauses
	bool	inner=false;
	bool	cross=false;
	bool	straight=false;
	bool	left=false;
	bool	right=false;
	bool	outer=false;
	bool	natural=false;
	bool	join=false;
	if (innerClause(ptr,newptr)) {
		inner=true;
	} else if (crossClause(ptr,newptr)) {
		cross=true;
	} else if (straightJoinClause(ptr,newptr)) {
		straight=true;
		join=true;
	} else if (leftClause(ptr,newptr)) {
		left=true;
		outer=outerClause(*newptr,newptr);
	} else if (rightClause(ptr,newptr)) {
		right=true;
		outer=outerClause(*newptr,newptr);
	} else if (naturalClause(ptr,newptr)) {
		natural=true;
		left=leftClause(*newptr,newptr);
		right=rightClause(*newptr,newptr);
		outer=outerClause(*newptr,newptr);
	}
	if (!straight) {
		join=joinClause(*newptr,newptr);
	}

	// do we have a join?
	if (!join) {
		*newptr=ptr;
		return false;
	}

	// create the node
	xmldomnode	*joinnode=newNode(currentnode,_join_clause);

	// create nodes for the various clauses
	if (inner) {
		newNode(joinnode,_inner);
	} else if (cross) {
		newNode(joinnode,_cross);
	} else if (straight) {
		newNode(joinnode,_straight_join);
	} else if (left) {
		newNode(joinnode,_left);
		if (outer) {
			newNode(joinnode,_outer);
		}
	} else if (right) {
		newNode(joinnode,_right);
		if (outer) {
			newNode(joinnode,_outer);
		}
	} else if (natural) {
		newNode(joinnode,_natural);
		if (left) {
			newNode(joinnode,_left);
		}
		if (right) {
			newNode(joinnode,_right);
		}
		if (outer) {
			newNode(joinnode,_outer);
		}
	}
	if (!straight) {
		newNode(joinnode,_join);
	}

	// table reference
	if (!parseTableReferences(joinnode,*newptr,newptr)) {
		debugPrintf("missing table references in join\n");
		error=true;
		return false;
	}

	// natural joins have no further clauses
	if (natural) {
		return true;
	}

	// some joins have "on" or "using" clauses
	return parseOn(joinnode,*newptr,newptr) ||
		parseJoinUsing(joinnode,*newptr,newptr) ||
		true;
}

const char *sqlreparser::_join_clause="join_clause";

bool sqlreparser::innerClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"inner ");
}

const char *sqlreparser::_inner="inner";

bool sqlreparser::crossClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"cross ");
}

const char *sqlreparser::_cross="cross";

const char *sqlreparser::_straight_join="straight_join";

bool sqlreparser::leftClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"left ");
}

const char *sqlreparser::_left="left";

bool sqlreparser::outerClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"outer ");
}

const char *sqlreparser::_right="right";

bool sqlreparser::rightClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"right ");
}

const char *sqlreparser::_outer="outer";

bool sqlreparser::naturalClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"natural ");
}

const char *sqlreparser::_natural="natural";

bool sqlreparser::joinClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"join ");
}

const char *sqlreparser::_join="join";

bool sqlreparser::parseOn(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for on
	if (!onClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*onnode=newNode(currentnode,_on);

	return parseWhereClauseTerms(onnode,*newptr,newptr);
}

bool sqlreparser::onClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"on ");
}

const char *sqlreparser::_on="on";

bool sqlreparser::parseJoinUsing(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for using
	if (!joinUsingClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*joinusingnode=newNode(currentnode,_join_using);

	// get the column name list
	if (!parseColumnNameList(joinusingnode,*newptr,newptr)) {
		debugPrintf("missing column name list\n");
		error=true;
		return false;
	}
	return true;
}

bool sqlreparser::joinUsingClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"using ");
}

const char *sqlreparser::_join_using="join_using";

bool sqlreparser::parseWhere(xmldomnode *currentnode,
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

bool sqlreparser::whereClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"where ");
}

const char *sqlreparser::_where="where";

bool sqlreparser::parseHaving(xmldomnode *currentnode,
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

bool sqlreparser::havingClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"having ");
}

const char *sqlreparser::_having="having";

bool sqlreparser::parseWhereClauseTerms(xmldomnode *currentnode,
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

bool sqlreparser::parseAnd(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!andClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_and);
	return true;
}

bool sqlreparser::andClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"and ");
}

const char *sqlreparser::_and="and";

bool sqlreparser::parseOr(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!orClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_or);
	return true;
}

bool sqlreparser::orClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"or ");
}

const char *sqlreparser::_or="or";

bool sqlreparser::parseWhereClauseTerm(xmldomnode *currentnode,
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

const char *sqlreparser::_group="group";

bool sqlreparser::parseComparison(xmldomnode *currentnode,
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
		parseMatches(comparisonnode,*newptr,newptr) ||
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

		// get the escape clause, if there is one
		parseEscape(comparisonnode,*newptr,newptr);

		return true;
	}

	// if the term was a boolean value or function returning a boolean
	// then there might not be a comparator
	return true;
}

const char *sqlreparser::_comparison="comparison";

bool sqlreparser::notClause(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"not ");
}                                   

const char *sqlreparser::_not="not";

bool sqlreparser::parseBetween(xmldomnode *currentnode,
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

bool sqlreparser::betweenClause(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"between ");
}                                   

const char *sqlreparser::_between="between";

bool sqlreparser::parseIn(xmldomnode *currentnode,
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

bool sqlreparser::inClause(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"in ");
}                                   

const char *sqlreparser::_in="in";

bool sqlreparser::parseInSet(xmldomnode *currentnode,
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

const char *sqlreparser::_in_set_item="in_set_item";

bool sqlreparser::parseExists(xmldomnode *currentnode,
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

bool sqlreparser::existsClause(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"exists");
}                                   

const char *sqlreparser::_exists="exists";

bool sqlreparser::parseIs(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!is(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_is);
	return true;
}

bool sqlreparser::is(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"is ");
}                                   

const char *sqlreparser::_is="is";

bool sqlreparser::parseLike(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!like(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_like);
	return true;
}

bool sqlreparser::like(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"like ");
}                                   

const char *sqlreparser::_like="like";

bool sqlreparser::parseMatches(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!matches(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_matches);
	return true;
}

bool sqlreparser::matches(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"matches ");
}                                   

const char *sqlreparser::_matches="matches";

bool sqlreparser::parseNullSafeEquals(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!nullSafeEquals(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_null_safe_equals);
	return true;
}

bool sqlreparser::nullSafeEquals(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"<=>");
}                                   

const char *sqlreparser::_null_safe_equals="null_safe_equals";

bool sqlreparser::parseEquals(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!equals(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_equals);
	return true;
}

bool sqlreparser::parseNotEquals(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!notEquals(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_not_equals);
	return true;
}
                   
const char *sqlreparser::_not_equals="notequals";

bool sqlreparser::parseLessThan(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!lessThan(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_less_than);
	return true;
}

const char *sqlreparser::_less_than="lessthan";

bool sqlreparser::parseGreaterThan(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!greaterThan(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_greater_than);
	return true;
}

const char *sqlreparser::_greater_than="greaterthan";

bool sqlreparser::parseLessThanOrEqualTo(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!lessThanOrEqualTo(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_less_than_or_equal_to);
	return true;
}

const char *sqlreparser::_less_than_or_equal_to="lessthanorequalto";

bool sqlreparser::parseGreaterThanOrEqualTo(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!greaterThanOrEqualTo(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_greater_than_or_equal_to);
	return true;
}

const char *sqlreparser::_greater_than_or_equal_to="greaterthanorequalto";

bool sqlreparser::parseEscape(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!escape(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_escape);
	return parseTerm(currentnode,*newptr,newptr);
}

bool sqlreparser::escape(const char *ptr, const char **newptr) {
        debugFunction();
        return comparePart(ptr,newptr,"escape ");
}                                   

const char *sqlreparser::_escape="escape";

bool sqlreparser::parseExpression(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	return parseExpression(currentnode,ptr,newptr,false);
}

bool sqlreparser::parseExpression(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					bool ingroup) {
	debugFunction();

	// create the node
	xmldomnode	*expressionnode=newNode(currentnode,_expression);

	// if we're in an expression group, then the expression could be an
	// entire select query, check for that first
	*newptr=ptr;
	if (ingroup && parseSelect(expressionnode,*newptr,newptr)) {
		return true;
	}

	// otherwise...

	// the expression could be any number of terms, separated by operators
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
			if (!parseExpression(groupnode,*newptr,newptr,true)) {
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

const char *sqlreparser::_expression="expression";

bool sqlreparser::parseUnaryOperator(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	return (parseNot(currentnode,ptr,newptr) ||
		parseDistinct(currentnode,ptr,newptr) ||
		parseCompliment(currentnode,ptr,newptr) ||
		parseInverse(currentnode,ptr,newptr) ||
		parseNegative(currentnode,ptr,newptr));
}

bool sqlreparser::parseNot(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!notClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_not);
	return true;
}

bool sqlreparser::parseCompliment(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!compliment(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_compliment);
	return true;
}

const char *sqlreparser::_compliment="compliment";

bool sqlreparser::parseInverse(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!inverse(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_inverse);
	return true;
}

const char *sqlreparser::_inverse="inverse";

bool sqlreparser::parseNegative(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!minus(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_negative);
	return true;
}

const char *sqlreparser::_negative="negative";

bool sqlreparser::parseBinaryOperator(xmldomnode *currentnode,
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

bool sqlreparser::parseTimes(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!times(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_times);
	return true;
}

const char *sqlreparser::_times="times";

bool sqlreparser::parseDividedBy(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!dividedBy(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_divided_by);
	return true;
}

const char *sqlreparser::_divided_by="divided_by";

bool sqlreparser::parseModulo(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!modulo(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_modulo);
	return true;
}

const char *sqlreparser::_modulo="modulo";

bool sqlreparser::parsePlus(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!plus(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_plus);
	return true;
}

const char *sqlreparser::_plus="plus";

bool sqlreparser::parseMinus(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!minus(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_minus);
	return true;
}

const char *sqlreparser::_minus="minus";

bool sqlreparser::parseLogicalAnd(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!logicalAnd(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_logical_and);
	return true;
}

const char *sqlreparser::_logical_and="logical_and";

bool sqlreparser::parseLogicalOr(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!logicalOr(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_logical_or);
	return true;
}

const char *sqlreparser::_logical_or="logical_or";

bool sqlreparser::parseBitwiseAnd(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!bitwiseAnd(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_bitwise_and);
	return true;
}

const char *sqlreparser::_bitwise_and="bitwise_and";

bool sqlreparser::parseBitwiseOr(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!bitwiseOr(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_bitwise_or);
	return true;
}

const char *sqlreparser::_bitwise_or="bitwise_or";

bool sqlreparser::parseBitwiseXor(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!bitwiseXor(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_bitwise_xor);
	return true;
}

const char *sqlreparser::_bitwise_xor="bitwise_xor";

bool sqlreparser::parseTerm(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// first, check to see if it's an interval qualifier
	if (parseIntervalQualifier(currentnode,ptr,newptr)) {
		return true;
	}

	// next check for an exists clause
	if (parseExists(currentnode,*newptr,newptr)) {
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

	// a term might be followed with "as <type>"
	// eg: cast(col1 as varchar2(200))
	if (parseAs(currentnode,*newptr,newptr)) {
		parseType(currentnode,*newptr,newptr);
	}


	// clean up
	delete[] term;

	return retval;
}

const char *sqlreparser::_number="number";
const char *sqlreparser::_string_literal="string_literal";
const char *sqlreparser::_bind_variable="bind_variable";

bool sqlreparser::parseIntervalQualifier(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// save the starting point, in case this fails
	const char	*startptr=ptr;

	// create the node
	xmldomnode	*iqnode=new xmldomnode(tree,
					currentnode->getNullNode(),
					TAG_XMLDOMNODETYPE,
					_interval_qualifier,NULL);

	// look for a time component, to, and another time component
	bool	retval=parseTimeComponent(iqnode,ptr,newptr,
						_from,_from_precision,
						_from_scale) &&
			parseTo(iqnode,*newptr,newptr) &&
			parseTimeComponent(iqnode,*newptr,newptr,
						_to,_to_precision,_to_scale);

	// if everything went well, attach the node,
	// otherwise reset the string pointer and delete it
	if (retval) {
		currentnode->appendChild(iqnode);
	} else {
		*newptr=ptr;
		delete iqnode;
	}

	// restore newptr if this failed
	if (!retval) {
		*newptr=startptr;
	}

	return retval;
}

const char *sqlreparser::_interval_qualifier="interval_qualifier";

bool sqlreparser::parseTo(xmldomnode *currentnode,
				const char *ptr,
				const char **newptr) {
	debugFunction();
	return toClause(ptr,newptr);
}

bool sqlreparser::toClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"to ");
}

const char *sqlreparser::_to="to";

bool sqlreparser::parseTimeComponent(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					const char *timecomponent,
					const char *precision,
					const char *scale) {
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

	// precision
	char	*number=getVerbatim(*newptr,newptr);
	if (charstring::isNumber(number)) {
		currentnode->setAttributeValue(precision,number);
		delete[] number;
	} else {
		delete[] number;
		return false;
	}

	// scale
	if (comma(*newptr,newptr)) {
		number=getVerbatim(*newptr,newptr);
		if (charstring::isNumber(number)) {
			currentnode->setAttributeValue(scale,number);
			delete[] number;
		} else {
			delete[] number;
			return false;
		}
	}
	return rightParen(*newptr,newptr);
}

const char *sqlreparser::_from_precision="fromprecision";
const char *sqlreparser::_from_scale="fromscale";
const char *sqlreparser::_to_precision="toprecision";
const char *sqlreparser::_to_scale="toscale";

bool sqlreparser::parseUnquotedLiteral(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// buffer to store the unquoted literal
	stringbuffer	unqoutedliteral;

	// get and append characters until we hit a comma or right paren
	const char	*chptr=NULL;
	for (chptr=ptr; *chptr && *chptr!=',' && *chptr!=')'; chptr++) {
		unqoutedliteral.append(*chptr);
	}

	// set newptr
	*newptr=chptr;

	// add a new node containing the unqouted literal
	newNode(currentnode,
		sqlreparser::_string_literal,
		unqoutedliteral.getString());

	return true;
}

bool sqlreparser::parseColumnOrFunction(xmldomnode *currentnode,
						const char *name,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// some date/time functions have unquoted date/time literals as
	// their only argument, check for those
	bool	unqoutedliteral=
		(!charstring::compareIgnoringCase(name,"datetime") ||
		!charstring::compareIgnoringCase(name,"interval"));

	// Check for an outer join operator "(+)" just so we don't confuse it
	// with function parameters.  In either case, reset newptr afterwards.
	// If we found one, we'll actually parse it later.
	bool	outerjoin=outerJoinOperatorClause(ptr,newptr);
	*newptr=ptr;

	// functions generally have parameters or at least empty parameters
	if (!outerjoin && leftParen(*newptr,newptr)) {

		// FIXME: split dot-delimited function names

		// create the nodes
		xmldomnode	*functionnode=
				newNode(currentnode,_function,name);
		xmldomnode	*paramsnode=
				newNode(functionnode,_parameters);

		// parse parameters
		for (;;) {

			// bail if we ran off the end of the string
			if (!(**newptr)) {
				debugPrintf("ran off the end of the string\n");	
				error=true;
				return false;
			}

			// bail when we find a right paren
			if (rightParen(*newptr,newptr)) {

				// some databases allow an interval qualifier
				// immediately after a function, test for that
				parseIntervalQualifier(currentnode,
							*newptr,newptr);

				// check for an outer join operator here too
				parseOuterJoinOperator(currentnode,
							*newptr,newptr);
				return true;
			}

			// create the node
			xmldomnode	*paramnode=
					newNode(paramsnode,_parameter);

			// parse the unquoted literal or expression
			if (unqoutedliteral) {

				// parse the unquoted literal
				parseUnquotedLiteral(paramnode,*newptr,newptr);

				// don't look for it on the next pass
				unqoutedliteral=false;
			} else {

				// parse the expression
				parseExpression(paramnode,*newptr,newptr);
			}

			// skip any commas
			comma(*newptr,newptr);
		}
	}

	// it's either a special function without parameters or a column name...

	// FIXME: split dot-delimited function names


	// some db's have special functions with no parameters
	// (eg. sysdate, current_date, today, etc.)
	// if it's not one of those then it's just a regular column
	const char	*type=(specialFunctionName(name))?
				_function:_column_reference;

	// create the nodes
	if (type==_function) {
		newNode(currentnode,type,name);
	} else {
		xmldomnode	*newnode=newNode(currentnode,type);
		splitColumnName(newnode,name);
	}

	// some databases allow an interval qualifier immediately
	// after a function or column, test for that
	parseIntervalQualifier(currentnode,*newptr,newptr);

	// check for an outer join operator here too
	parseOuterJoinOperator(currentnode,*newptr,newptr);

	return true;
}

const char *sqlreparser::_column_reference="column_reference";
const char *sqlreparser::_function="function";
const char *sqlreparser::_parameters="parameters";
const char *sqlreparser::_parameter="parameter";

static const char *defaultspecialfunctionnames[]={
	// date functions
	"sysdate",
	"systimestamp",
	"current_date",
	"current",
	"call_dtime",
	"today",
	// other functions...
	NULL
};

bool sqlreparser::specialFunctionName(const char *name) {

	const char * const	*names=defaultspecialfunctionnames;
	for (uint64_t i=0; names[i]; i++) {
		if (!charstring::compare(name,names[i])) {
			return true;
		}
	}
	return false;
}

void sqlreparser::splitColumnName(xmldomnode *currentnode, const char *name) {

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

const char *sqlreparser::_column_name_database="column_name_database";
const char *sqlreparser::_column_name_schema="column_name_schema";
const char *sqlreparser::_column_name_table="column_name_table";
const char *sqlreparser::_column_name_column="column_name_column";

bool sqlreparser::parseOuterJoinOperator(xmldomnode *currentnode,
					const char *ptr, const char **newptr) {
	debugFunction();
	if (!outerJoinOperatorClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_outer_join_operator);
	return true;
}

bool sqlreparser::outerJoinOperatorClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char	*start=ptr;
	if (!(leftParen(ptr,newptr) &&
		plus(*newptr,newptr) &&
		rightParen(*newptr,newptr))) {
		*newptr=start;
		return false;
	}
	return true;
}

const char *sqlreparser::_outer_join_operator="outer_join_operator";

bool sqlreparser::parseLock(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a lock clause
	if (!lockClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*locknode=newNode(currentnode,_lock);

	// table...
	if (!tableClause(*newptr,newptr)) {
		debugPrintf("missing table clause\n");
		error=true;
		return false;
	}

	// table node
	xmldomnode	*tablenode=newNode(locknode,_table);

	// table name
	if (!parseTableName(tablenode,*newptr,newptr)) {
		debugPrintf("missing table name\n");
		error=true;
		return false;
	}

	// in
	if (!inClause(*newptr,newptr)) {
		debugPrintf("missing in clause\n");
		error=true;
		return false;
	}
	newNode(tablenode,_in_mode);

	// lock mode
	if (!parseLockMode(locknode,*newptr,newptr)) {
		debugPrintf("invalid lock mode\n");
		error=true;
		return false;
	}

	// mode
	if (!parseMode(locknode,*newptr,newptr)) {
		debugPrintf("missing mode clause\n");
		error=true;
		return false;
	}

	// nowait
	parseNoWait(locknode,*newptr,newptr);

	parseRemainderVerbatim(locknode,*newptr,newptr);
	return true;
}

bool sqlreparser::lockClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"lock ");
}

const char *sqlreparser::_lock="lock";

const char *sqlreparser::_in_mode="in_mode";

bool sqlreparser::parseLockMode(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// lock mode
	if (!lockModeClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*lockmodenode=newNode(currentnode,_lock_mode);

	// value
	char	*value=getClause(ptr,*newptr);
	setAttribute(lockmodenode,_value,value);
	delete[] value;
	return true;
}

bool sqlreparser::lockModeClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char	*parts[]={
		"row share",
		"row exclusive",
		"share update",
		"share",
		"share row",
		"exclusive",
		"or exclusive",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlreparser::_lock_mode="lock_mode";

bool sqlreparser::parseMode(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!modeClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_mode);
	return true;
}

bool sqlreparser::modeClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"mode");
}

const char *sqlreparser::_mode="mode";

bool sqlreparser::parseNoWait(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!noWaitClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_nowait);
	return true;
}

bool sqlreparser::noWaitClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"nowait");
}

const char *sqlreparser::_nowait="nowait";

bool sqlreparser::parseShow(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {

	debugFunction();

	// look for a show clause
	if (!showClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*shownode=newNode(currentnode,_show);

	// get the thing we're showing
	char	*value=getWord(*newptr,newptr);
	if (value) {
		setAttribute(shownode,_value,value);
		delete[] value;
	}

	parseRemainderVerbatim(shownode,*newptr,newptr);
	return true;
}

bool sqlreparser::showClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"show ");
}

const char *sqlreparser::_show="show";

bool sqlreparser::write(stringbuffer *output) {
	debugFunction();
	return write(tree->getRootNode()->getFirstTagChild(),output,false);
}

bool sqlreparser::write(xmldomnode *node,
				stringbuffer *output,
				bool omitsiblings) {
	debugFunction();
	if (omitsiblings) {
		return write(node,output);
	}
	for (xmldomnode *n=node; !n->isNullNode(); n=n->getNextTagSibling()) {
		if (!write(n,output)) {
			return false;
		}
	}
	return true;
}

bool sqlreparser::write(xmldomnode *node, stringbuffer *output) {
	debugFunction();

	// handle the element
	if (!handleStart(node,output)) {
		return false;
	}

	// append a space afterward if it's got children
	if (!dontAppendSpace(output) &&
		!node->getFirstTagChild()->isNullNode()) {
		space(output);
	}

	// handle the children
	for (xmldomnode *child=node->getFirstTagChild();
		!child->isNullNode(); child=child->getNextTagSibling()) {
		if (!write(child,output)) {
			return false;
		}
	}

	// some elements need to do things on the back end, so handle that too
	if (!handleEnd(node,output)) {
		return false;
	}

	// append a space afterward if it's got siblings
	if (!dontAppendSpace(output) &&
		!node->getNextTagSibling()->isNullNode()) {
		space(output);
	}
	return true;
}

bool sqlreparser::handleStart(xmldomnode *node, stringbuffer *output) {
	debugFunction();

	if (!charstring::compare(
		node->getAttributeValue("supported"),"false")) {
		return true;
	}

	const char	*nodename=node->getName();

	// generic...
	if (!charstring::compare(nodename,_table_name_database)) {
		return tableNameDatabase(node,output);
	}
	if (!charstring::compare(nodename,_table_name_schema)) {
		return tableNameSchema(node,output);
	}
	if (!charstring::compare(nodename,_table_name_table)) {
		return tableNameTable(node,output);
	}
	if (!charstring::compare(nodename,_name)) {
		return name(node,output);
	}
	if (!charstring::compare(nodename,_type)) {
		return type(node,output);
	}
	if (!charstring::compare(nodename,_size)) {
		return size(node,output);
	}
	if (!charstring::compare(nodename,_value)) {
		return value(node,output);
	}
	if (!charstring::compare(nodename,_verbatim)) {
		return verbatim(node,output);
	}

	// create query...
	if (!charstring::compare(nodename,_create)) {
		return createQuery(node,output);
	}
	if (!charstring::compare(nodename,_global)) {
		return global(node,output);
	}
	if (!charstring::compare(nodename,_temporary)) {
		return temporary(node,output);
	}

	// table...
	if (!charstring::compare(nodename,_table)) {
		return table(node,output);
	}
	if (!charstring::compare(nodename,_if_not_exists)) {
		return ifNotExists(node,output);
	}

	// index...
	if (!charstring::compare(nodename,_fulltext)) {
		return fulltext(node,output);
	}
	if (!charstring::compare(nodename,_spatial)) {
		return spatial(node,output);
	}
	if (!charstring::compare(nodename,_index)) {
		return index(node,output);
	}
	if (!charstring::compare(nodename,_index_name_database)) {
		return indexNameDatabase(node,output);
	}
	if (!charstring::compare(nodename,_index_name_schema)) {
		return indexNameSchema(node,output);
	}
	if (!charstring::compare(nodename,_index_name_index)) {
		return indexNameIndex(node,output);
	}
	if (!charstring::compare(nodename,_btree)) {
		return btree(node,output);
	}
	if (!charstring::compare(nodename,_hash)) {
		return hash(node,output);
	}
	if (!charstring::compare(nodename,_key_block_size)) {
		return keyBlockSize(node,output);
	}
	if (!charstring::compare(nodename,_with_parser)) {
		return withParser(node,output);
	}

	// synonym...
	if (!charstring::compare(nodename,_synonym)) {
		return synonym(node,output);
	}
	if (!charstring::compare(nodename,_for)) {
		return forClause(node,output);
	}
	if (!charstring::compare(nodename,_object_name_database)) {
		return objectNameDatabase(node,output);
	}
	if (!charstring::compare(nodename,_object_name_schema)) {
		return objectNameSchema(node,output);
	}
	if (!charstring::compare(nodename,_object_name_object)) {
		return objectNameObject(node,output);
	}

	// column definitions...
	if (!charstring::compare(nodename,_columns)) {
		return columns(node,output);
	}
	if (!charstring::compare(nodename,_column)) {
		return column(node,output);
	}
	if (!charstring::compare(nodename,_values)) {
		return values(node,output);
	}
	if (!charstring::compare(nodename,_length)) {
		return length(node,output);
	}
	if (!charstring::compare(nodename,_scale)) {
		return scale(node,output);
	}

	// constraints...
	if (!charstring::compare(nodename,_constraints)) {
		return constraints(node,output);
	}
	if (!charstring::compare(nodename,_unsigned)) {
		return unsignedConstraint(node,output);
	}
	if (!charstring::compare(nodename,_zerofill)) {
		return zerofill(node,output);
	}
	if (!charstring::compare(nodename,_binary)) {
		return binary(node,output);
	}
	if (!charstring::compare(nodename,_character_set)) {
		return characterSet(node,output);
	}
	if (!charstring::compare(nodename,_collate)) {
		return collate(node,output);
	}
	if (!charstring::compare(nodename,_null)) {
		return nullable(node,output);
	}
	if (!charstring::compare(nodename,_not_null)) {
		return notNull(node,output);
	}
	if (!charstring::compare(nodename,_default)) {
		return defaultValue(node,output);
	}
	if (!charstring::compare(nodename,_auto_increment)) {
		return autoIncrement(node,output);
	}
	if (!charstring::compare(nodename,_identity)) {
		return identity(node,output);
	}
	if (!charstring::compare(nodename,_unique_key)) {
		return uniqueKey(node,output);
	}
	if (!charstring::compare(nodename,_primary_key)) {
		return primaryKey(node,output);
	}
	if (!charstring::compare(nodename,_key)) {
		return key(node,output);
	}
	if (!charstring::compare(nodename,_comment)) {
		return comment(node,output);
	}
	if (!charstring::compare(nodename,_column_format)) {
		return columnFormat(node,output);
	}
	if (!charstring::compare(nodename,_references)) {
		return references(node,output);
	}
	if (!charstring::compare(nodename,_match)) {
		return match(node,output);
	}
	if (!charstring::compare(nodename,_on_delete)) {
		return onDelete(node,output);
	}
	if (!charstring::compare(nodename,_on_update)) {
		return onUpdate(node,output);
	}
	if (!charstring::compare(nodename,_on_commit)) {
		return onCommit(node,output);
	}
	if (!charstring::compare(nodename,_as)) {
		return as(node,output);
	}
	if (!charstring::compare(nodename,_constraint)) {
		return constraint(node,output);
	}
	if (!charstring::compare(nodename,_foreign_key)) {
		return foreignKey(node,output);
	}
	if (!charstring::compare(nodename,_check)) {
		return check(node,output);
	}


	// drop...
	if (!charstring::compare(nodename,_drop)) {
		return dropQuery(node,output);
	}
	if (!charstring::compare(nodename,_if_exists)) {
		return ifExists(node,output);
	}
	if (!charstring::compare(nodename,_restrict_clause)) {
		return restrictClause(node,output);
	}
	if (!charstring::compare(nodename,_cascade)) {
		return cascade(node,output);
	}
	if (!charstring::compare(nodename,
				_cascade_constraints_clause)) {
		return cascadeConstraintsClause(node,output);
	}


	// insert...
	if (!charstring::compare(nodename,_insert)) {
		return insertQuery(node,output);
	}
	if (!charstring::compare(nodename,_insert_into)) {
		return insertInto(node,output);
	}
	if (!charstring::compare(nodename,_insert_values_clause)) {
		return insertValuesClause(node,output);
	}
	if (!charstring::compare(nodename,_insert_value_clause)) {
		return insertValueClause(node,output);
	}
	if (!charstring::compare(nodename,_insert_value)) {
		return insertValue(node,output);


	// update...
	}
	if (!charstring::compare(nodename,_update)) {
		return updateQuery(node,output);
	}
	if (!charstring::compare(nodename,_update_set)) {
		return updateSet(node,output);
	}
	if (!charstring::compare(nodename,_assignment)) {
		return assignment(node,output);
	}
	if (!charstring::compare(nodename,_equals)) {
		return equals(node,output);
	}


	// delete...
	if (!charstring::compare(nodename,_delete)) {
		return deleteQuery(node,output);
	}
	if (!charstring::compare(nodename,_delete_from)) {
		return deleteFrom(node,output);
	}
	if (!charstring::compare(nodename,_using)) {
		return usingClause(node,output);
	}


	// select...
	if (!charstring::compare(nodename,_select)) {
		return selectQuery(node,output);
	}
	if (!charstring::compare(nodename,_select_expressions)) {
		return selectExpressions(node,output);
	}
	if (!charstring::compare(nodename,_select_expression)) {
		return selectExpression(node,output);
	}
	if (!charstring::compare(nodename,_sub_select)) {
		return subSelect(node,output);
	}
	if (!charstring::compare(nodename,_union)) {
		return unionClause(node,output);
	}
	if (!charstring::compare(nodename,_all)) {
		return all(node,output);
	}
	if (!charstring::compare(nodename,_alias)) {
		return alias(node,output);
	}
	if (!charstring::compare(nodename,_unique)) {
		return unique(node,output);
	}
	if (!charstring::compare(nodename,_distinct)) {
		return distinct(node,output);
	}
	if (!charstring::compare(nodename,_from)) {
		return from(node,output);
	}
	if (!charstring::compare(nodename,_table_references)) {
		return tableReferences(node,output);
	}
	if (!charstring::compare(nodename,_table_reference)) {
		return tableReference(node,output);
	}
	if (!charstring::compare(nodename,_join_clause)) {
		return joinClause(node,output);
	}
	if (!charstring::compare(nodename,_inner)) {
		return inner(node,output);
	}
	if (!charstring::compare(nodename,_cross)) {
		return cross(node,output);
	}
	if (!charstring::compare(nodename,_straight_join)) {
		return straightJoin(node,output);
	}
	if (!charstring::compare(nodename,_left)) {
		return left(node,output);
	}
	if (!charstring::compare(nodename,_right)) {
		return right(node,output);
	}
	if (!charstring::compare(nodename,_outer)) {
		return outer(node,output);
	}
	if (!charstring::compare(nodename,_natural)) {
		return natural(node,output);
	}
	if (!charstring::compare(nodename,_join)) {
		return join(node,output);
	}
	if (!charstring::compare(nodename,_on)) {
		return on(node,output);
	}
	if (!charstring::compare(nodename,_join_using)) {
		return joinUsing(node,output);
	}
	if (!charstring::compare(nodename,_where)) {
		return where(node,output);
	}
	if (!charstring::compare(nodename,_and)) {
		return andClause(node,output);
	}
	if (!charstring::compare(nodename,_or)) {
		return orClause(node,output);
	}
	if (!charstring::compare(nodename,_group)) {
		return group(node,output);
	}
	if (!charstring::compare(nodename,_comparison)) {
		return comparison(node,output);
	}
	if (!charstring::compare(nodename,_not)) {
		return notClause(node,output);
	}
	if (!charstring::compare(nodename,_between)) {
		return between(node,output);
	}
	if (!charstring::compare(nodename,_in)) {
		return in(node,output);
	}
	if (!charstring::compare(nodename,_in_set_item)) {
		return inSetItem(node,output);
	}
	if (!charstring::compare(nodename,_exists)) {
		return exists(node,output);
	}
	if (!charstring::compare(nodename,_is)) {
		return is(node,output);
	}
	if (!charstring::compare(nodename,_like)) {
		return like(node,output);
	}
	if (!charstring::compare(nodename,_matches)) {
		return matches(node,output);
	}
	if (!charstring::compare(nodename,_null_safe_equals)) {
		return nullSafeEquals(node,output);
	}
	if (!charstring::compare(nodename,_not_equals)) {
		return notEquals(node,output);
	}
	if (!charstring::compare(nodename,_less_than)) {
		return lessThan(node,output);
	}
	if (!charstring::compare(nodename,_greater_than)) {
		return greaterThan(node,output);
	}
	if (!charstring::compare(nodename,_less_than_or_equal_to)) {
		return lessThanOrEqualTo(node,output);
	}
	if (!charstring::compare(nodename,
				_greater_than_or_equal_to)) {
		return greaterThanOrEqualTo(node,output);
	}
	if (!charstring::compare(nodename,_escape)) {
		return escape(node,output);
	}
	if (!charstring::compare(nodename,_expression)) {
		return expression(node,output);
	}
	if (!charstring::compare(nodename,_interval_qualifier)) {
		return intervalQualifier(node,output);
	}
	if (!charstring::compare(nodename,_outer_join_operator)) {
		return outerJoinOperator(node,output);
	}
	if (!charstring::compare(nodename,_compliment)) {
		return compliment(node,output);
	}
	if (!charstring::compare(nodename,_inverse)) {
		return inverse(node,output);
	}
	if (!charstring::compare(nodename,_negative)) {
		return negative(node,output);
	}
	if (!charstring::compare(nodename,_plus)) {
		return plus(node,output);
	}
	if (!charstring::compare(nodename,_minus)) {
		return minus(node,output);
	}
	if (!charstring::compare(nodename,_times)) {
		return times(node,output);
	}
	if (!charstring::compare(nodename,_divided_by)) {
		return dividedBy(node,output);
	}
	if (!charstring::compare(nodename,_modulo)) {
		return modulo(node,output);
	}
	if (!charstring::compare(nodename,_logical_and)) {
		return logicalAnd(node,output);
	}
	if (!charstring::compare(nodename,_logical_or)) {
		return logicalOr(node,output);
	}
	if (!charstring::compare(nodename,_bitwise_and)) {
		return bitwiseAnd(node,output);
	}
	if (!charstring::compare(nodename,_bitwise_or)) {
		return bitwiseOr(node,output);
	}
	if (!charstring::compare(nodename,_bitwise_xor)) {
		return bitwiseXor(node,output);
	}
	if (!charstring::compare(nodename,_number)) {
		return number(node,output);
	}
	if (!charstring::compare(nodename,_string_literal)) {
		return stringLiteral(node,output);
	}
	if (!charstring::compare(nodename,_bind_variable)) {
		return bindVariable(node,output);
	}
	if (!charstring::compare(nodename,_column_reference)) {
		return columnReference(node,output);
	}
	if (!charstring::compare(nodename,_column_name_database)) {
		return columnNameDatabase(node,output);
	}
	if (!charstring::compare(nodename,_column_name_schema)) {
		return columnNameSchema(node,output);
	}
	if (!charstring::compare(nodename,_column_name_table)) {
		return columnNameTable(node,output);
	}
	if (!charstring::compare(nodename,_column_name_column)) {
		return columnNameColumn(node,output);
	}
	if (!charstring::compare(nodename,_function)) {
		return function(node,output);
	}
	if (!charstring::compare(nodename,_parameters)) {
		return parameters(node,output);
	}
	if (!charstring::compare(nodename,_parameter)) {
		return parameter(node,output);
	}
	if (!charstring::compare(nodename,_group_by)) {
		return groupBy(node,output);
	}
	if (!charstring::compare(nodename,_group_by_item)) {
		return groupByItem(node,output);
	}
	if (!charstring::compare(nodename,_with_rollup)) {
		return withRollup(node,output);
	}
	if (!charstring::compare(nodename,_having)) {
		return having(node,output);
	}
	if (!charstring::compare(nodename,_order_by)) {
		return orderBy(node,output);
	}
	if (!charstring::compare(nodename,_order_by_item)) {
		return orderByItem(node,output);
	}
	if (!charstring::compare(nodename,_asc)) {
		return asc(node,output);
	}
	if (!charstring::compare(nodename,_desc)) {
		return desc(node,output);
	}
	if (!charstring::compare(nodename,_limit)) {
		return limit(node,output);
	}
	if (!charstring::compare(nodename,_select_into)) {
		return selectInto(node,output);
	}
	if (!charstring::compare(nodename,_procedure)) {
		return procedure(node,output);
	}
	if (!charstring::compare(nodename,_for_update)) {
		return forUpdate(node,output);
	}
	if (!charstring::compare(nodename,_set)) {
		return setQuery(node,output);
	}
	if (!charstring::compare(nodename,_set_global)) {
		return setGlobal(node,output);
	}
	if (!charstring::compare(nodename,_set_session)) {
		return setSession(node,output);
	}
	if (!charstring::compare(nodename,_transaction)) {
		return transaction(node,output);
	}
	if (!charstring::compare(nodename,_limit)) {
		return limit(node,output);
	}
	if (!charstring::compare(nodename,_select_into)) {
		return selectInto(node,output);
	}
	if (!charstring::compare(nodename,_procedure)) {
		return procedure(node,output);
	}
	if (!charstring::compare(nodename,_for_update)) {
		return forUpdate(node,output);
	}
	if (!charstring::compare(nodename,_set)) {
		return setQuery(node,output);
	}
	if (!charstring::compare(nodename,_set_global)) {
		return setGlobal(node,output);
	}
	if (!charstring::compare(nodename,_set_session)) {
		return setSession(node,output);
	}
	if (!charstring::compare(nodename,_transaction)) {
		return transaction(node,output);
	}
	if (!charstring::compare(nodename,_isolation_level)) {
		return isolationLevel(node,output);
	}
	if (!charstring::compare(nodename,_lock)) {
		return lockQuery(node,output);
	}
	if (!charstring::compare(nodename,_in_mode)) {
		return inMode(node,output);
	}
	if (!charstring::compare(nodename,_lock_mode)) {
		return lockMode(node,output);
	}
	if (!charstring::compare(nodename,_mode)) {
		return mode(node,output);
	}
	if (!charstring::compare(nodename,_nowait)) {
		return noWait(node,output);
	}
	if (!charstring::compare(nodename,_show)) {
		return show(node,output);
	}
	return true;
}

bool sqlreparser::handleEnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();

	if (!charstring::compare(
		node->getAttributeValue("supported"),"false")) {
		return true;
	}

	const char	*nodename=node->getName();

	// generic...
	if (!charstring::compare(nodename,_type)) {
		return endType(node,output);
	}
	if (!charstring::compare(nodename,_size)) {
		return endSize(node,output);
	}

	// column definitions...
	if (!charstring::compare(nodename,_columns)) {
		return endColumns(node,output);
	}
	if (!charstring::compare(nodename,_column)) {
		return endColumn(node,output);
	}
	if (!charstring::compare(nodename,_values)) {
		return endValues(node,output);
	}
	if (!charstring::compare(nodename,_check)) {
		return endCheck(node,output);
	}

	// drop...
	if (!charstring::compare(nodename,
					_table_name_list_item)) {
		return endTableNameListItem(node,output);
	}

	// insert...
	if (!charstring::compare(nodename,
					_insert_values_clause)) {
		return endInsertValuesClause(node,output);
	}
	if (!charstring::compare(nodename,
					_insert_value_clause)) {
		return endInsertValueClause(node,output);
	}
	if (!charstring::compare(nodename,_insert_value)) {
		return endInsertValue(node,output);
	}

	// update...
	if (!charstring::compare(nodename,_assignment)) {
		return endAssignment(node,output);
	}

	// select...
	if (!charstring::compare(nodename,
					_select_expression)) {
		return endSelectExpression(node,output);
	}
	if (!charstring::compare(nodename,_sub_select)) {
		return endSubSelect(node,output);
	}
	if (!charstring::compare(nodename,_group)) {
		return endGroup(node,output);
	}
	if (!charstring::compare(nodename,_parameters)) {
		return endParameters(node,output);
	}
	if (!charstring::compare(nodename,_parameter)) {
		return endParameter(node,output);
	}
	if (!charstring::compare(nodename,_table_reference)) {
		return endTableReference(node,output);
	}
	if (!charstring::compare(nodename,_join_clause)) {
		return endJoinClause(node,output);
	}
	if (!charstring::compare(nodename,_order_by_item)) {
		return endOrderByItem(node,output);
	}
	if (!charstring::compare(nodename,_group_by_item)) {
		return endGroupByItem(node,output);
	}
	if (!charstring::compare(nodename,_in)) {
		return endIn(node,output);
	}
	if (!charstring::compare(nodename,_in_set_item)) {
		return endInSetItem(node,output);
	}
	if (!charstring::compare(nodename,_exists)) {
		return endExists(node,output);
	}
	return true;
}

bool sqlreparser::outputValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(node->getAttributeValue(_value));
	return true;
}

bool sqlreparser::space(stringbuffer *output) {
	debugFunction();
	output->append(" ");
	return true;
}

bool sqlreparser::comma(stringbuffer *output) {
	debugFunction();
	output->append(",");
	return true;
}

bool sqlreparser::period(stringbuffer *output) {
	debugFunction();
	output->append(".");
	return true;
}

bool sqlreparser::leftParen(stringbuffer *output) {
	debugFunction();
	output->append("(");
	return true;
}

bool sqlreparser::rightParen(stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlreparser::hasSibling(xmldomnode *node) {
	debugFunction();
	return !node->getNextTagSibling()->isNullNode();
}

bool sqlreparser::dontAppendSpace(stringbuffer *output) {
	debugFunction();
	size_t	length=output->getStringLength();
	return (length &&
		character::inSet(output->getString()[length-1]," .(,"));
}

bool sqlreparser::createQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("create");
	return true;
}

bool sqlreparser::table(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("table");
	return true;
}

bool sqlreparser::global(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("global");
	return true;
}

bool sqlreparser::temporary(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("temporary");
	return true;
}

bool sqlreparser::ifNotExists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("if not exists");
	return true;
}

bool sqlreparser::columns(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlreparser::endColumns(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlreparser::column(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::endColumn(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlreparser::values(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlreparser::endValues(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlreparser::length(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::scale(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	comma(output);
	outputValue(node,output);
	return true;
}

bool sqlreparser::constraints(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::unsignedConstraint(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unsigned");
	return true;
}

bool sqlreparser::zerofill(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("zerofill");
	return true;
}

bool sqlreparser::binary(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("binary");
	return true;
}

bool sqlreparser::characterSet(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("character set ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::collate(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("collate ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::nullable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("null");
	return true;
}

bool sqlreparser::notNull(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("not null");
	return true;
}

bool sqlreparser::defaultValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("default ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::autoIncrement(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("auto_increment");
	return true;
}

bool sqlreparser::identity(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("identity");
	return true;
}

bool sqlreparser::uniqueKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unique key");
	return true;
}

bool sqlreparser::primaryKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("primary key");
	return true;
}

bool sqlreparser::key(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("key");
	return true;
}

bool sqlreparser::comment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("comment ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::columnFormat(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("column_format ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::references(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("references ");
	return true;
}

bool sqlreparser::endReferences(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::match(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("match ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::onDelete(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on delete ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::onUpdate(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on update ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::onCommit(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on commit ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::as(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("as ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::constraint(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	const char	*name=node->getAttributeValue("value");
	if (name) {
		output->append("constraint ")->append(name);
	}
	return true;
}

bool sqlreparser::foreignKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("foreign key ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::check(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("check (");
	return true;
}

bool sqlreparser::endCheck(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlreparser::withNoLog(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("with no log ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::fulltext(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("fulltext ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::spatial(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("spatial ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::index(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("index ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::indexNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlreparser::indexNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlreparser::indexNameIndex(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::btree(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("btree ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::hash(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("hash ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::keyBlockSize(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("key_block_size");
	if (!charstring::compare(node->getAttributeValue("equals"),"true")) {
		output->append("=");
	} else {
		output->append(" ");
	}
	outputValue(node,output);
	return true;
}

bool sqlreparser::withParser(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("with parser ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::synonym(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("synonym ");
	return true;
}

bool sqlreparser::forClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("for ");
	return true;
}

bool sqlreparser::objectNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlreparser::objectNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlreparser::objectNameObject(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::deleteQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("delete");
	return true;
}

bool sqlreparser::deleteFrom(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("from");
	return true;
}

bool sqlreparser::usingClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("using");
	return true;
}

bool sqlreparser::dropQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("drop");
	return true;
}

bool sqlreparser::ifExists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("if exists");
	return true;
}

bool sqlreparser::endTableNameListItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (!node->getNextSibling()->isNullNode()) {
		comma(output);
	}
	return true;
}

bool sqlreparser::restrictClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("restrict");
	return true;
}

bool sqlreparser::cascade(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("cascade");
	return true;
}

bool sqlreparser::cascadeConstraintsClause(xmldomnode *node,
						stringbuffer *output) {
	debugFunction();
	output->append("constraints");
	return true;
}

bool sqlreparser::tableNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlreparser::tableNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlreparser::tableNameTable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::name(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::type(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::endType(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::size(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlreparser::endSize(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlreparser::value(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlreparser::verbatim(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::insertQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("insert");
	return true;
}

bool sqlreparser::insertInto(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("into");
	return true;
}

bool sqlreparser::insertValuesClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("values (");
	return true;
}

bool sqlreparser::insertValueClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("value (");
	return true;
}

bool sqlreparser::insertValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::endInsertValuesClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlreparser::endInsertValueClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlreparser::endInsertValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlreparser::selectQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("select");
	return true;
}

bool sqlreparser::selectExpressions(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::selectExpression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::endSelectExpression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlreparser::subSelect(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlreparser::endSubSelect(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlreparser::unionClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("union");
	return true;
}

bool sqlreparser::all(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("all");
	return true;
}

bool sqlreparser::alias(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::unique(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unique");
	return true;
}

bool sqlreparser::distinct(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("distinct");
	return true;
}

bool sqlreparser::from(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("from");
	return true;
}

bool sqlreparser::tableReferences(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::tableReference(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::endTableReference(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// write a comma if the next node isn't a join clause
	xmldomnode	*next=node->getNextTagSibling();
	if (!next->isNullNode() &&
		charstring::compare(next->getName(),_join_clause)) {
		comma(output);
	}
	return true;
}

bool sqlreparser::joinClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::endJoinClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// write a comma if the next node isn't a join clause
	xmldomnode	*next=node->getNextTagSibling();
	if (!next->isNullNode() &&
		charstring::compare(next->getName(),_join_clause)) {
		comma(output);
	}
	return true;
}

bool sqlreparser::inner(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("inner");
	return true;
}

bool sqlreparser::cross(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("cross");
	return true;
}

bool sqlreparser::straightJoin(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("straight_join");
	return true;
}

bool sqlreparser::left(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("left");
	return true;
}

bool sqlreparser::right(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("right");
	return true;
}

bool sqlreparser::outer(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("outer");
	return true;
}

bool sqlreparser::natural(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("natural");
	return true;
}

bool sqlreparser::join(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("join");
	return true;
}

bool sqlreparser::on(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on");
	return true;
}

bool sqlreparser::joinUsing(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("using");
	return true;
}

bool sqlreparser::groupBy(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("group by");
	return true;
}

bool sqlreparser::groupByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::endGroupByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// write a comma if the next node is another group-by-item
	xmldomnode	*next=node->getNextTagSibling();
	if (!next->isNullNode() &&
		!charstring::compare(next->getName(),_group_by_item)) {
		comma(output);
	}
	return true;
}

bool sqlreparser::withRollup(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("with rollup");
	return true;
}

bool sqlreparser::having(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("having");
	return true;
}

bool sqlreparser::orderBy(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("order by");
	return true;
}

bool sqlreparser::orderByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::endOrderByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlreparser::asc(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("asc");
	return true;
}

bool sqlreparser::desc(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("desc");
	return true;
}

bool sqlreparser::limit(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("limit");
	return true;
}

bool sqlreparser::selectInto(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("into");
	return true;
}

bool sqlreparser::procedure(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("procedure");
	return true;
}

bool sqlreparser::forUpdate(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("for update");
	return true;
}

bool sqlreparser::where(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("where");
	return true;
}

bool sqlreparser::andClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("and");
	return true;
}

bool sqlreparser::orClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("or");
	return true;
}

bool sqlreparser::comparison(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::notClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("not");
	return true;
}

bool sqlreparser::group(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("(");
	return true;
}

bool sqlreparser::endGroup(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlreparser::between(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("between");
	return true;
}

bool sqlreparser::in(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("in ");
	leftParen(output);
	return true;
}

bool sqlreparser::endIn(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlreparser::inSetItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::endInSetItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlreparser::exists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("exists ");
	leftParen(output);
	return true;
}

bool sqlreparser::endExists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlreparser::is(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("is");
	return true;
}

bool sqlreparser::like(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("like");
	return true;
}

bool sqlreparser::matches(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("matches");
	return true;
}

bool sqlreparser::nullSafeEquals(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("<=>");
	return true;
}

bool sqlreparser::notEquals(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("!=");
	return true;
}

bool sqlreparser::lessThan(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("<");
	return true;
}

bool sqlreparser::greaterThan(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(">");
	return true;
}

bool sqlreparser::lessThanOrEqualTo(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("<=");
	return true;
}

bool sqlreparser::greaterThanOrEqualTo(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(">=");
	return true;
}

bool sqlreparser::escape(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("escape");
	return true;
}

bool sqlreparser::expression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::intervalQualifier(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(node->getAttributeValue(_from));
	const char	*precision=
			node->getAttributeValue(_from_precision);
	const char	*scale=
			node->getAttributeValue(_from_scale);
	if (precision) {
		leftParen(output);
		output->append(precision);
		if (scale) {
			comma(output);
			output->append(scale);
		}
		rightParen(output);
	}
	if (charstring::length(node->getAttributeValue(_to))) {
		output->append(" to ");
		output->append(node->getAttributeValue(_to));
		precision=node->getAttributeValue(_to_precision);
		scale=node->getAttributeValue(_to_scale);
		if (precision) {
			leftParen(output);
			output->append(precision);
			if (scale) {
				comma(output);
				output->append(scale);
			}
			rightParen(output);
		}
	}
	return true;
}

bool sqlreparser::outerJoinOperator(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("(+)");
	return true;
}

bool sqlreparser::compliment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("~");
	return true;
}

bool sqlreparser::inverse(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("!");
	return true;
}

bool sqlreparser::negative(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("-");
	return true;
}

bool sqlreparser::plus(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("+");
	return true;
}

bool sqlreparser::minus(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("-");
	return true;
}

bool sqlreparser::times(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("*");
	return true;
}

bool sqlreparser::dividedBy(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("/");
	return true;
}

bool sqlreparser::modulo(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("%");
	return true;
}

bool sqlreparser::bitwiseAnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("&");
	return true;
}

bool sqlreparser::bitwiseOr(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("|");
	return true;
}

bool sqlreparser::bitwiseXor(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("^");
	return true;
}

bool sqlreparser::logicalAnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("&&");
	return true;
}

bool sqlreparser::logicalOr(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("||");
	return true;
}

bool sqlreparser::number(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::stringLiteral(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::bindVariable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::columnReference(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::columnNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlreparser::columnNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlreparser::columnNameTable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlreparser::columnNameColumn(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::function(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::parameters(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlreparser::endParameters(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlreparser::parameter(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::endParameter(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlreparser::updateQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("update");
	return true;
}

bool sqlreparser::updateSet(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("set");
	return true;
}

bool sqlreparser::assignment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlreparser::endAssignment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		output->append(",");
	}
	return true;
}

bool sqlreparser::equals(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("=");
	return true;
}

bool sqlreparser::setQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("set");
	return true;
}

bool sqlreparser::setSession(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("session");
	return true;
}

bool sqlreparser::setGlobal(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("global");
	return true;
}

bool sqlreparser::transaction(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("transaction ");
	return true;
}

bool sqlreparser::isolationLevel(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("isolation level ");
	outputValue(node,output);
	return true;
}

bool sqlreparser::lockQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("lock");
	return true;
}

bool sqlreparser::inMode(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("in");
	return true;
}

bool sqlreparser::lockMode(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlreparser::mode(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("mode");
	return true;
}

bool sqlreparser::noWait(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("nowait");
	return true;
}

bool sqlreparser::show(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("show ");
	outputValue(node,output);
	return true;
}
