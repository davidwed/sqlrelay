// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>
#include <rudiments/snooze.h>

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
		parseCreateSynonym(createnode,*newptr,newptr)) {
		return true;
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

bool sqlparser::parseGlobal(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!globalClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_global);
	return true;
}

bool sqlparser::globalClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"global ");
}

const char *sqlparser::_global="global";

bool sqlparser::parseTemporary(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!temporaryClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_temporary);
	return true;
}

bool sqlparser::temporaryClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"temporary ",
		"temp ",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

const char *sqlparser::_temporary="temporary";

bool sqlparser::parseFulltext(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!fulltext(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_fulltext);
	return true;
}

bool sqlparser::fulltext(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"fulltext ");
}

const char *sqlparser::_fulltext="fulltext";

bool sqlparser::parseSpatial(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!spatial(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_spatial);
	return true;
}

bool sqlparser::spatial(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"spatial ");
}

const char *sqlparser::_spatial="spatial";

bool sqlparser::tableClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"table ");
}

bool sqlparser::parseCreateTable(xmldomnode *currentnode,
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

const char *sqlparser::_table="table";

bool sqlparser::parseIfNotExists(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
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
		return false;
	}

	// create new node
	xmldomnode	*columnsnode=newNode(currentnode,_columns);

	// column and constraint definitions
	for (;;) {

		// create column definition node
		xmldomnode	*coldefnode=newNode(columnsnode,
							_column);

		// column definition or constraint
		const char	*start=*newptr;
		if (!parseColumnDefinition(coldefnode,start,newptr) &&
			!parseConstraint(coldefnode,start,newptr)) {
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
	if (!parseTableName(referencesnode,*newptr,newptr)) {
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

bool sqlparser::parseWithNoLog(xmldomnode *currentnode,
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

const char *sqlparser::_with_no_log="with_no_log";

bool sqlparser::parseCreateIndex(xmldomnode *currentnode,
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

	// columns
	if (!leftParen(*newptr,newptr)) {
		debugPrintf("missing left paren\n");
		error=true;
		return false;
	}

	// column list
	if (!parseColumnNameList(indexnode,*newptr,newptr)) {
		debugPrintf("missing column list\n");
		error=true;
		return false;
	}

	// right paren
	if (!rightParen(*newptr,newptr)) {
		debugPrintf("missing right paren\n");
		error=true;
		return false;
	}

	// index type
	parseIndexType(indexnode,*newptr,newptr);

	return true;
}

bool sqlparser::indexClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"index ");
}

const char *sqlparser::_index="index";

bool sqlparser::parseIndexName(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	char	*indexname=getWord(ptr,newptr);
	splitDatabaseObjectName(currentnode,
				indexname,
				_index_name_database,
				_index_name_schema,
				_index_name_index);
	delete[] indexname;
	return true;
}

const char *sqlparser::_index_name_database="index_name_database";
const char *sqlparser::_index_name_schema="index_name_schema";
const char *sqlparser::_index_name_index="index_name_index";

bool sqlparser::parseIndexType(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// bail right away if we find an on clause, which is
	// the only thing that could follow the index type
	if (onClause(ptr,newptr)) {
		*newptr=ptr;
		return false;
	}

	// using
	if (!usingClause(*newptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*usingnode=newNode(currentnode,_using);

	// btree or hash
	if (parseBtree(usingnode,*newptr,newptr) ||
		parseHash(usingnode,*newptr,newptr)) {
		return true;
	}

	// some other, unrecognized index type
	parseVerbatim(usingnode,*newptr,newptr);
	return false;
}

bool sqlparser::parseBtree(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!btree(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_btree);
	return true;
}

bool sqlparser::btree(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"btree ");
}

const char *sqlparser::_btree="btree";

bool sqlparser::parseHash(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!hash(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_hash);
	return true;
}

bool sqlparser::hash(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"hash ");
}

const char *sqlparser::_hash="hash";

bool sqlparser::parseOnClause(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!onClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_on);
	return true;
}

bool sqlparser::parseConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();

	// FIXME: implement this...

	return false;
}

bool sqlparser::parseCreateSynonym(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// synonym
	if (!synonymClause(ptr,newptr)) {
		return true;
	}

	// create new node
	xmldomnode	*synonymnode=newNode(currentnode,_synonym);

	// synonym name
	if (!parseName(synonymnode,*newptr,newptr)) {
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
	if (!parseName(synonymnode,*newptr,newptr)) {
		debugPrintf("missing object name\n");
		error=true;
		return false;
	}

	return true;
}

bool sqlparser::synonymClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"synonym ");
}

const char *sqlparser::_synonym="synonym";

bool sqlparser::parseFor(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!forClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_for);
	return true;
}

bool sqlparser::forClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"for ");
}

const char *sqlparser::_for="for";
