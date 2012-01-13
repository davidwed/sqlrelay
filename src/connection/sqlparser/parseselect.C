// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>

bool sqlparser::parseSelect(xmldomnode *currentnode,
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

		uint16_t	parenlevel=0;
		foundunion=false;

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

bool sqlparser::selectClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"select ");
}

const char *sqlparser::_select="select";

bool sqlparser::parseSubSelects(xmldomnode *currentnode,
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
			debugPrintf("missing subselect\n");
			error=true;
			return false;
		}

		// right paren
		if (!rightParen(*newptr,newptr)) {
			debugPrintf("missing right paren after subselect\n");
			error=true;
			return false;
		}

		// It gets very complicated here.  There could be an alias,
		// optionally with an "as" before it, there could be a union,
		// or there could be nothing.  Since different DB's do their
		// aliases differently, it's hard to say what the rules for
		// a valid alias are and difficult to determine whether one
		// is present.

		// look for unions again
		if (parseUnion(currentnode,*newptr,newptr)) {
			continue;
		}

		// as clause (optional)
		const char	*beforeas=*newptr;
		bool		foundas=asClause(*newptr,newptr);

		// look for unions again
		if (parseUnion(currentnode,*newptr,newptr)) {
			if (foundas) {
				// apparently the "as" was the alias
				parseAlias(currentnode,
						beforeas,&beforeas,false);
			}
			continue;
		}

		// look for stuff that comes after a subselect
		// FIXME: I really need to look for joins here too
		const char	*beforealias=*newptr;
		if (comma(*newptr,newptr) || rightParen(*newptr,newptr)) {
			*newptr=beforealias;
			if (foundas) {
				// apparently the "as" was the alias
				parseAlias(currentnode,*newptr,newptr,false);
			}
			return true;
		}

		// if we didn't find a union then
		// the next word might be an alias
		if (!parseAlias(currentnode,*newptr,newptr,foundas)) {
			// if we found something other than an alias but
			// we found an as clause earlier, then again, the
			// "as" clause was the alias
			if (foundas) {
				parseAlias(currentnode,
						beforeas,&beforeas,false);
			}
		}

		// look for unions again
		if (parseUnion(currentnode,*newptr,newptr)) {
			continue;
		}

		// if there was no union then we ought to be done
		// FIXME: actually there could be mysql index hints here
		return true;
	}
}

const char *sqlparser::_sub_select="sub_select";

bool sqlparser::parseAlias(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					bool as) {
	debugFunction();
	bool	retval=false;
	char	*alias=getWord(ptr,newptr);
	if (charstring::length(alias)) {
		if (as) {
			currentnode=newNode(currentnode,_as);
		}
		newNode(currentnode,_alias,alias);
		retval=true;
	}
	delete[] alias;
	return retval;
}

const char *sqlparser::_alias="alias";

bool sqlparser::parseUnion(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!unionClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_union);
	return true;
}

bool sqlparser::unionClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"union ");
}

const char *sqlparser::_union="union";

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

bool sqlparser::groupByClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"group by ");
}

const char *sqlparser::_group_by="group_by";
const char *sqlparser::_group_by_item="group_by_item";

bool sqlparser::parseWithRollup(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!withRollupClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_with_rollup);
	return true;
}

bool sqlparser::withRollupClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"with rollup");
}

const char *sqlparser::_with_rollup="with_rollup";

bool sqlparser::parseOrderBy(xmldomnode *currentnode,
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

bool sqlparser::orderByClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"order by ");
}

const char *sqlparser::_order_by="order_by";
const char *sqlparser::_order_by_item="order_by_item";

bool sqlparser::parseAsc(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!asc(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_asc);
	return true;
}

bool sqlparser::asc(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"asc");
}

const char *sqlparser::_asc="asc";

bool sqlparser::parseDesc(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!desc(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_desc);
	return true;
}

bool sqlparser::desc(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"desc");
}

const char *sqlparser::_desc="desc";

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
