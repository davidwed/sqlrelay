// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqlelement.h>
#include <sqltranslatordebug.h>

bool sqlparser::parseCreate(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create clause
	if (!createClause(ptr,newptr)) {
		debugPrintf("missing create clause\n");
		return false;
	}

	// create the node
	xmldomnode	*createnode=
			newNode(currentnode,sqlelement::_create);

	// temporary
	parseCreateTemporary(createnode,*newptr,newptr);

	// table, index, etc..
	if (tableClause(*newptr,newptr)) {
		return parseCreateTable(createnode,*newptr,newptr);
	}

	// for now we only support tables
	parseRemainderVerbatim(createnode,*newptr,newptr);
	return false;
}

bool sqlparser::parseCreateTemporary(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	if (!temporaryClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_create_temporary);
	return true;
}

bool sqlparser::parseCreateTable(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// create new node
	xmldomnode	*tablenode=newNode(currentnode,sqlelement::_table);

	// if not exists
	parseIfNotExists(tablenode,ptr,newptr);

	// table name
	parseTableName(tablenode,*newptr,newptr);

	// column and constrain definitions
	if (!parseColumnAndConstraintDefinitions(tablenode,*newptr,newptr)) {
		return false;
	}

	// on commit (optional)
	parseOnCommit(tablenode,*newptr,newptr);

	// store anything trailing off the end verbatim
	parseRemainderVerbatim(tablenode,*newptr,newptr);

	return true;
}

bool sqlparser::parseIfNotExists(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	if (!ifNotExistsClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_if_not_exists);
	return true;
}

bool sqlparser::parseColumnAndConstraintDefinitions(
						xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// left paren
	if (!leftParen(ptr,newptr)) {
		debugPrintf("missing left paren\n");
		return false;
	}

	// create new node
	xmldomnode	*columnsnode=newNode(currentnode,sqlelement::_columns);

	// column and constraint definitions
	for (;;) {

		// create column definition node
		xmldomnode	*coldefnode=newNode(columnsnode,
							sqlelement::_column);

		// FIXME: handle constraints here

		// column definition
		parseColumnDefinition(coldefnode,*newptr,newptr);

		// comma
		if (comma(*newptr,newptr)) {
			continue;
		}

		// right paren
		if (rightParen(*newptr,newptr)) {

			// consume any spaces after the
			// column/constraint definitions
			space(*newptr,newptr);
			return true;
		}
	}
}

bool sqlparser::parseColumnDefinition(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// column name
	if (!parseColumnName(currentnode,ptr,newptr)) {
		debugPrintf("missing column name\n");
		return false;
	}

	// column type
	if (!parseType(currentnode,*newptr,newptr)) {
		debugPrintf("missing column type\n");
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
		// but we need to stay on it, so we'll compare newptr directly
		// rather than using rightParen()
		if (**newptr==',' || **newptr==')') {
			break;
		}

		// create constraints node
		if (!constraintsnode) {
			constraintsnode=
				newNode(currentnode,sqlelement::_constraints);
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
		if (parseVerbatim(constraintsnode,*newptr,newptr)) {
			space(*newptr,newptr);
		} else {
			break;
		}
	}

	return true;
}

bool sqlparser::parseUnsigned(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!unsignedClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_unsigned);
	space(*newptr,newptr);
	return true;
}

bool sqlparser::parseZeroFill(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!zeroFillClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_zerofill);
	space(*newptr,newptr);
	return true;
}

bool sqlparser::parseBinary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!binaryClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_binary);
	space(*newptr,newptr);
	return true;
}

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
	newNode(currentnode,sqlelement::_character_set,word);
	delete[] word;
	return true;
}

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
	newNode(currentnode,sqlelement::_collate,word);
	delete[] word;
	return true;
}

bool sqlparser::parseNull(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!nullClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_null);
	space(*newptr,newptr);
	return true;
}

bool sqlparser::parseNotNull(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!notNullClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_not_null);
	space(*newptr,newptr);
	return true;
}

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
	newNode(currentnode,sqlelement::_default,value);
	delete[] value;

	space(*newptr,newptr);
	return true;
}

bool sqlparser::parseAutoIncrement(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!autoIncrementClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_auto_increment);
	space(*newptr,newptr);
	return true;
}

bool sqlparser::parseUniqueKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!uniqueKeyClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_unique_key);
	space(*newptr,newptr);
	return true;
}

bool sqlparser::parsePrimaryKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!primaryKeyClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_primary_key);
	space(*newptr,newptr);
	return true;
}

bool sqlparser::parseKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!keyClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,sqlelement::_key);
	space(*newptr,newptr);
	return true;
}

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
	newNode(currentnode,sqlelement::_comment,value);
	delete[] value;

	space(*newptr,newptr);
	return true;
}

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
	newNode(currentnode,sqlelement::_column_format,word);
	delete[] word;
	return true;
}

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
						sqlelement::_references);

	// table name
	if (!parseTableName(referencesnode,*newptr,newptr)) {
		debugPrintf("missing table name\n");
		return false;
	}

	// left paren
	if (!leftParen(*newptr,newptr)) {
		debugPrintf("missing left paren\n");
		return false;
	}

	// column names
	if (!parseReferenceColumns(referencesnode,*newptr,newptr)) {
		return false;
	}

	// right paren
	if (!rightParen(*newptr,newptr)) {
		debugPrintf("missing right paren\n");
		return false;
	}

	// space
	space(*newptr,newptr);

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

bool sqlparser::parseReferenceColumns(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// create new node
	xmldomnode	*columnsnode=newNode(currentnode,sqlelement::_columns);

	*newptr=ptr;
	for (;;) {

		// get the column
		char	*column=getWord(*newptr,newptr);
		if (!column) {
			debugPrintf("missing right paren\n");
			return false;
		}

		// create new node
		xmldomnode	*columnnode=
				newNode(columnsnode,sqlelement::_column);
		newNode(columnnode,sqlelement::_name,column);

		// clean up
		delete[] column;

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
	newNode(currentnode,sqlelement::_match,word);
	delete[] word;
	return true;
}

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
			newNode(currentnode,sqlelement::_on_delete);

	// reference option
	if (!parseReferenceOption(ondeletenode,*newptr,newptr)) {
		return false;
	}
	return true;
}

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
			newNode(currentnode,sqlelement::_on_update);

	// reference option
	if (!parseReferenceOption(onupdatenode,*newptr,newptr)) {
		return false;
	}
	return true;
}

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
	setAttribute(currentnode,sqlelement::_value,value);

	// clean up
	delete[] value;

	// consume any trailing spaces
	space(*newptr,newptr);

	// success
	return true;
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
			newNode(currentnode,sqlelement::_on_commit);

	// on commit option
	const char	*startptr=*newptr;
	if (!onCommitOptionClause(startptr,newptr)) {
		return false;
	}

	// get the option
	char	*value=getClause(startptr,*newptr);

	// store it in the value attribute
	setAttribute(oncommitnode,sqlelement::_value,value);

	// clean up
	delete[] value;

	// consume any trailing spaces
	space(*newptr,newptr);

	// success
	return true;
}
