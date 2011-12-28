// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>

bool sqlparser::parseSelect(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();

	// look for a select clause
	if (!selectClause(ptr,newptr)) {
		return false;
	}

	// create the node
	xmldomnode	*selectnode=newNode(currentnode,_select);

	// parse the select clauses
	for (;;) {

		// look for known options
		if (parseUnique(selectnode,*newptr,newptr) ||
			parseDistinct(selectnode,*newptr,newptr) ||
			parseFrom(selectnode,*newptr,newptr) ||
			parseWhere(selectnode,*newptr,newptr) ||
			parseGroupBy(selectnode,*newptr,newptr) ||
			parseHaving(selectnode,*newptr,newptr) ||
			parseOrderBy(selectnode,*newptr,newptr) ||
			parseLimit(selectnode,*newptr,newptr) ||
			parseProcedure(selectnode,*newptr,newptr) ||
			parseSelectInto(selectnode,*newptr,newptr) ||
			parseForUpdate(selectnode,*newptr,newptr)) {
			continue;
		}

		// If we didn't encounter one of the known options
		// then there must be something in there that we don't
		// understand.  It needs to be copied verbatim until we run
		// into something that we do understand.
		if (parseVerbatim(selectnode,*newptr,newptr)) {

			// if we find a comma, append that too
			if (comma(*newptr,newptr)) {
				newNode(selectnode,_verbatim,",");
			}

		} else {
			break;
		}
	}

	return true;
}

bool sqlparser::selectClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"select ");
}

const char *sqlparser::_select="select";

bool sqlparser::parseUnique(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!uniqueClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_unique);
	return true;
}

bool sqlparser::uniqueClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"unique ");
}

const char *sqlparser::_unique="unique";

bool sqlparser::parseDistinct(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!distinctClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_distinct);
	return true;
}

bool sqlparser::distinctClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"distinct ");
}

const char *sqlparser::_distinct="distinct";

bool sqlparser::parseFrom(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!fromClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_from);
	return true;
}

bool sqlparser::fromClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"from ");
}

const char *sqlparser::_from="from";

bool sqlparser::parseGroupBy(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!groupByClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_group_by);
	return true;
}

bool sqlparser::groupByClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"group by ");
}

const char *sqlparser::_group_by="group_by";

bool sqlparser::parseHaving(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!havingClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_having);
	return true;
}

bool sqlparser::havingClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"having ");
}

const char *sqlparser::_having="having";

bool sqlparser::parseOrderBy(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!orderByClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_order_by);
	return true;
}

bool sqlparser::orderByClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"order by ");
}

const char *sqlparser::_order_by="order_by";

bool sqlparser::parseLimit(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!limitClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_limit);
	return true;
}

bool sqlparser::limitClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"limit ");
}

const char *sqlparser::_limit="limit";

bool sqlparser::parseProcedure(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!procedureClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_procedure);
	return true;
}

bool sqlparser::procedureClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"procedure ");
}

const char *sqlparser::_procedure="procedure";

bool sqlparser::parseSelectInto(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!selectIntoClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_select_into);
	return true;
}

bool sqlparser::selectIntoClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"into ");
}

const char *sqlparser::_select_into="select_into";

bool sqlparser::parseForUpdate(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!forUpdateClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_for_update);
	return true;
}

bool sqlparser::forUpdateClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"for update ");
}

const char *sqlparser::_for_update="for_update";
