// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>
#include <rudiments/snooze.h>

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

			// look for a from clause
			// (do this first so the word "from" doesn't get
			// mistaken for a term in an expression
			const char	*beforefrom=*newptr;
			if (fromClause(*newptr,newptr)) {
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

			// if we got here then there was no from clause
			debugPrintf("missing from clause\n");
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
				parseSelectInto(selectnode,*newptr,newptr) ||
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

bool sqlparser::selectClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"select ");
}

const char *sqlparser::_select="select";
const char *sqlparser::_select_expressions="select_expressions";
const char *sqlparser::_select_expression="select_expression";

bool sqlparser::parseSelectExpressionAlias(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	return parseAlias(currentnode,ptr,newptr,false);
}

bool sqlparser::parseAlias(xmldomnode *currentnode,
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
	if (!charstring::compareIgnoringCase(word,sqlparser::_as)) {
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
		!charstring::compareIgnoringCase(word,sqlparser::_from) ||
		// for clause
		!charstring::compareIgnoringCase(word,"for") ||
		// order by clause
		!charstring::compareIgnoringCase(word,"order") ||
		// grop by clause
		!charstring::compareIgnoringCase(word,"group") ||
		(subselect &&
		// where clause
		(!charstring::compareIgnoringCase(word,sqlparser::_where) ||
		// join clauses
		!charstring::compareIgnoringCase(word,sqlparser::_inner) ||
		!charstring::compareIgnoringCase(word,sqlparser::_cross) ||
		!charstring::compareIgnoringCase(
					word,sqlparser::_straight_join) ||
		!charstring::compareIgnoringCase(word,sqlparser::_left) ||
		!charstring::compareIgnoringCase(word,sqlparser::_right) ||
		!charstring::compareIgnoringCase(word,sqlparser::_natural) ||
		!charstring::compareIgnoringCase(word,sqlparser::_on) ||
		!charstring::compareIgnoringCase(word,sqlparser::_using) ||
		// union clause
		!charstring::compareIgnoringCase(word,sqlparser::_union)))) {

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

const char *sqlparser::_sub_select="sub_select";

bool sqlparser::parseSubSelectAlias(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	return parseAlias(currentnode,ptr,newptr,true);
}

const char *sqlparser::_alias="alias";

bool sqlparser::parseUnion(xmldomnode *currentnode,
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

bool sqlparser::unionClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"union ");
}

const char *sqlparser::_union="union";

bool sqlparser::parseAll(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!allClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_all);
	return true;
}

bool sqlparser::allClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"all ");
}

const char *sqlparser::_all="all";

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

bool sqlparser::parseDistinctRow(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!distinctRowClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_distinct_row);
	return true;
}

bool sqlparser::distinctRowClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"distinctrow ");
}

const char *sqlparser::_distinct_row="distinct_row";

bool sqlparser::parseHighPriority(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!highPriorityClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_high_priority);
	return true;
}

bool sqlparser::highPriorityClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"high_priority ");
}

const char *sqlparser::_high_priority="high_priority";

bool sqlparser::parseStraightJoinSelectOption(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr) {
	debugFunction();
	if (!straightJoinClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_straight_join_select_option);
	return true;
}

bool sqlparser::straightJoinClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"straight_join ");
}

const char *sqlparser::_straight_join_select_option=
					"straight_join_select_option";

bool sqlparser::parseSqlSmallResult(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!sqlSmallResultClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_sql_small_result);
	return true;
}

bool sqlparser::sqlSmallResultClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"sql_small_result ");
}

const char *sqlparser::_sql_small_result="sql_small_result";

bool sqlparser::parseSqlBigResult(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!sqlBigResultClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_sql_big_result);
	return true;
}

bool sqlparser::sqlBigResultClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"sql_big_result ");
}

const char *sqlparser::_sql_big_result="sql_big_result";

bool sqlparser::parseSqlBufferResult(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!sqlBufferResultClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_sql_buffer_result);
	return true;
}

bool sqlparser::sqlBufferResultClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"sql_buffer_result ");
}

const char *sqlparser::_sql_buffer_result="sql_buffer_result";

bool sqlparser::parseSqlCache(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!sqlCacheClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_sql_cache);
	return true;
}

bool sqlparser::sqlCacheClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"sql_cache ");
}

const char *sqlparser::_sql_cache="sql_cache";

bool sqlparser::parseSqlNoCache(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!sqlNoCacheClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_sql_no_cache);
	return true;
}

bool sqlparser::sqlNoCacheClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"sql_no_cache ");
}

const char *sqlparser::_sql_no_cache="sql_no_cache";

bool sqlparser::parseSqlCalcFoundRows(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr) {
	debugFunction();
	if (!sqlCalcFoundRowsClause(ptr,newptr)) {
		return false;
	}
	newNode(currentnode,_sql_calc_found_rows);
	return true;
}

bool sqlparser::sqlCalcFoundRowsClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"sql_calc_found_rows ");
}

const char *sqlparser::_sql_calc_found_rows="sql_calc_found_rows";

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
	return comparePart(ptr,newptr,"for update");
}

const char *sqlparser::_for_update="for_update";
