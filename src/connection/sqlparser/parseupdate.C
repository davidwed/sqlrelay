// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>
#include <rudiments/character.h>

bool sqlparser::parseUpdate(xmldomnode *currentnode,
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

bool sqlparser::updateClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"update ");
}

const char *sqlparser::_update="update";

bool sqlparser::parseUpdateSet(xmldomnode *currentnode,
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

bool sqlparser::updateSetClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"set ");
}

const char *sqlparser::_update_set="update_set";
const char *sqlparser::_assignment="assignment";
const char *sqlparser::_equals="equals";
