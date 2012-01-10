// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>

bool sqlparser::parseCreate(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create clause
	if (!createClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*createnode=newNode(currentnode,_create);

	// temporary
	parseCreateTemporary(createnode,*newptr,newptr);

	// table, index, etc..
	if (tableClause(*newptr,newptr)) {
		return parseCreateTable(createnode,*newptr,newptr);
	}

	// for now we only support tables
	parseRemainderVerbatim(createnode,*newptr,newptr);
	return true;
}

bool sqlparser::createClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"create ");
}

const char *sqlparser::_create="create";

bool sqlparser::parseCreateTemporary(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	if (!temporaryClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_create_temporary);
	return true;
}

bool sqlparser::temporaryClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"temporary ",
		"temp ",
		"global temp ",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlparser::_create_temporary="create_temporary";

bool sqlparser::tableClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"table ");
}

bool sqlparser::parseCreateTable(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create new node
	xmldomnode	*tablenode=newNode(currentnode,_table);

	// if not exists
	parseIfNotExists(tablenode,ptr,newptr);

	// table name
	parseName(tablenode,*newptr,newptr);

	// column and constrain definitions
	// (optional for create ... as select ...
	parseColumnAndConstraintDefinitions(tablenode,*newptr,newptr);

	// on commit (optional)
	parseOnCommit(tablenode,*newptr,newptr);

	// parse the remaining clauses
	for (;;) {

		// known clauses
		if (parseAs(tablenode,*newptr,newptr)) {
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

const char *sqlparser::_table="table";

bool sqlparser::parseIfNotExists(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	if (!ifNotExistsClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_if_not_exists);
	return true;
}

bool sqlparser::ifNotExistsClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"if not exists ");
}

const char *sqlparser::_if_not_exists="if_not_exists";

bool sqlparser::parseColumnAndConstraintDefinitions(
						xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// left paren
	if (!leftParen(ptr,newptr)) {
		debugPrintf("missing left paren\n");
		error=true;
		return false;
	}

	// create new node
	xmldomnode	*columnsnode=newNode(currentnode,_columns);

	// column and constraint definitions
	for (;;) {

		// create column definition node
		xmldomnode	*coldefnode=newNode(columnsnode,
							_column);

		// FIXME: handle constraints here

		// column definition
		parseColumnDefinition(coldefnode,*newptr,newptr);

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

const char *sqlparser::_columns="columns";
const char *sqlparser::_column="column";

bool sqlparser::parseColumnDefinition(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// column name
	if (!parseName(currentnode,ptr,newptr)) {
		debugPrintf("missing column name\n");
		error=true;
		return false;
	}

	// column type
	if (!parseType(currentnode,*newptr,newptr)) {
		debugPrintf("missing column type\n");
		error=true;
		return false;
	}

	// constraints
	parseConstraints(currentnode,*newptr,newptr);

	// success
	return true;
}

bool sqlparser::parseConstraints(xmldomnode *currentnode,
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

const char *sqlparser::_constraints="constraints";

bool sqlparser::parseUnsigned(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!unsignedClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_unsigned);
	return true;
}

bool sqlparser::unsignedClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"unsigned");
}

const char *sqlparser::_unsigned="unsigned";

bool sqlparser::parseZeroFill(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!zeroFillClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_zerofill);
	return true;
}

bool sqlparser::zeroFillClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"zerofill");
}

const char *sqlparser::_zerofill="zerofill";

bool sqlparser::parseBinary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!binaryClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_binary);
	return true;
}

bool sqlparser::binaryClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"binary");
}

const char *sqlparser::_binary="binary";

bool sqlparser::parseCharacterSet(xmldomnode *currentnode,
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

bool sqlparser::characterSetClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"character set ",
		"char set ",
		"charset ",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlparser::_character_set="character_set";

bool sqlparser::parseCollate(xmldomnode *currentnode,
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

bool sqlparser::collateClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"collate");
}

const char *sqlparser::_collate="collate";

bool sqlparser::parseNull(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!nullClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_null);
	return true;
}

bool sqlparser::nullClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"nullable",
		"null",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlparser::_null="null";

bool sqlparser::parseNotNull(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!notNullClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_not_null);
	return true;
}

bool sqlparser::notNullClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"not nullable",
		"not null",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlparser::_not_null="not_null";

bool sqlparser::parseDefault(xmldomnode *currentnode,
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

bool sqlparser::defaultClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"default value ",
		"default ",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlparser::_default="default";

bool sqlparser::parseAutoIncrement(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!autoIncrementClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_auto_increment);
	return true;
}

bool sqlparser::autoIncrementClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"auto_increment");
}

const char *sqlparser::_auto_increment="auto_increment";

bool sqlparser::parseUniqueKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!uniqueKeyClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_unique_key);
	return true;
}

bool sqlparser::uniqueKeyClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"unique key",
		"unique",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlparser::_unique_key="unique_key";

bool sqlparser::parsePrimaryKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!primaryKeyClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_primary_key);
	return true;
}

bool sqlparser::primaryKeyClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"primary key");
}

const char *sqlparser::_primary_key="primary_key";

bool sqlparser::parseKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!keyClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_key);
	return true;
}

bool sqlparser::keyClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"key");
}

const char *sqlparser::_key="key";

bool sqlparser::parseComment(xmldomnode *currentnode,
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

bool sqlparser::commentClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"comment ");
}

const char *sqlparser::_comment="comment";

bool sqlparser::parseColumnFormat(xmldomnode *currentnode,
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

bool sqlparser::columnFormatClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"column_format ");
}

const char *sqlparser::_column_format="column_format";

bool sqlparser::parseReferenceDefinition(xmldomnode *currentnode,
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
	if (!parseName(referencesnode,*newptr,newptr)) {
		debugPrintf("missing table name\n");
		error=true;
		return false;
	}

	// left paren
	if (!leftParen(*newptr,newptr)) {
		debugPrintf("missing left paren\n");
		error=true;
		return false;
	}

	// column names
	if (!parseColumnNameList(referencesnode,*newptr,newptr)) {
		return false;
	}

	// right paren
	if (!rightParen(*newptr,newptr)) {
		debugPrintf("missing right paren\n");
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

bool sqlparser::referencesClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"references ");
}

const char *sqlparser::_references="references";

bool sqlparser::parseColumnNameList(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// create new node
	xmldomnode	*columnsnode=newNode(currentnode,_columns);

	*newptr=ptr;
	for (;;) {

		// get the column
		char	*column=getWord(*newptr,newptr);
		if (!column) {
			debugPrintf("missing right paren\n");
			error=true;
			return false;
		}

		// create new node
		xmldomnode	*columnnode=
				newNode(columnsnode,_column);
		newNode(columnnode,_name,column);

		// clean up
		delete[] column;

		// skip the next comma
		comma(*newptr,newptr);

		// if we hit a right parentheses then we're done, but we need
		// to stay on it, so we'll reset the pointer afterwards if we
		// find one
		const char	*before=*newptr;
		if (rightParen(*newptr,newptr)) {
			*newptr=before;
			return true;
		}
	}
}

bool sqlparser::parseMatch(xmldomnode *currentnode,
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

bool sqlparser::matchClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"match ");
}

const char *sqlparser::_match="match";

bool sqlparser::parseOnDelete(xmldomnode *currentnode,
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

bool sqlparser::onDeleteClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"on delete ");
}

const char *sqlparser::_on_delete="on_delete";

bool sqlparser::parseOnUpdate(xmldomnode *currentnode,
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

bool sqlparser::onUpdateClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"on update ");
}

const char *sqlparser::_on_update="on_update";

bool sqlparser::parseReferenceOption(xmldomnode *currentnode,
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

bool sqlparser::referenceOptionClause(const char *ptr, const char **newptr) {
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

bool sqlparser::parseOnCommit(xmldomnode *currentnode,
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

bool sqlparser::onCommitClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"on commit ");
}

const char *sqlparser::_on_commit="on_commit";

bool sqlparser::onCommitOptionClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"delete rows",
		"preserve rows",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlparser::parseAs(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	if (!asClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_as);
	return true;
}

bool sqlparser::asClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"as ");
}

const char *sqlparser::_as="as";
