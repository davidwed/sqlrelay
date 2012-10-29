// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

bool sqlparser::parseFrom(xmldomnode *currentnode,
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

bool sqlparser::fromClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"from ");
}

const char *sqlparser::_from="from";

bool sqlparser::parseTableReferences(xmldomnode *currentnode,
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

const char *sqlparser::_table_references="table_references";

bool sqlparser::parseTableReference(xmldomnode *currentnode,
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

const char *sqlparser::_table_reference="table_reference";

bool sqlparser::parseJoin(xmldomnode *currentnode,
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

const char *sqlparser::_join_clause="join_clause";

bool sqlparser::innerClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"inner ");
}

const char *sqlparser::_inner="inner";

bool sqlparser::crossClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"cross ");
}

const char *sqlparser::_cross="cross";

const char *sqlparser::_straight_join="straight_join";

bool sqlparser::leftClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"left ");
}

const char *sqlparser::_left="left";

bool sqlparser::outerClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"outer ");
}

const char *sqlparser::_right="right";

bool sqlparser::rightClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"right ");
}

const char *sqlparser::_outer="outer";

bool sqlparser::naturalClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"natural ");
}

const char *sqlparser::_natural="natural";

bool sqlparser::joinClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"join ");
}

const char *sqlparser::_join="join";

bool sqlparser::parseOn(xmldomnode *currentnode,
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

bool sqlparser::onClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"on ");
}

const char *sqlparser::_on="on";

bool sqlparser::parseJoinUsing(xmldomnode *currentnode,
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

bool sqlparser::joinUsingClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"using ");
}

const char *sqlparser::_join_using="join_using";
