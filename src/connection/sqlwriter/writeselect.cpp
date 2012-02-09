// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <sqlparser.h>
#include <debugprint.h>

bool sqlwriter::selectQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("select");
	return true;
}

bool sqlwriter::selectExpressions(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::selectExpression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endSelectExpression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::subSelect(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlwriter::endSubSelect(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::unionClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("union");
	return true;
}

bool sqlwriter::all(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("all");
	return true;
}

bool sqlwriter::alias(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::unique(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unique");
	return true;
}

bool sqlwriter::distinct(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("distinct");
	return true;
}

bool sqlwriter::from(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("from");
	return true;
}

bool sqlwriter::tableReferences(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::tableReference(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endTableReference(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// write a comma if the next node isn't a join clause
	xmldomnode	*next=node->getNextTagSibling();
	if (!next->isNullNode() &&
		charstring::compare(next->getName(),sqlparser::_join_clause)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::joinClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endJoinClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// write a comma if the next node isn't a join clause
	xmldomnode	*next=node->getNextTagSibling();
	if (!next->isNullNode() &&
		charstring::compare(next->getName(),sqlparser::_join_clause)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::inner(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("inner");
	return true;
}

bool sqlwriter::cross(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("cross");
	return true;
}

bool sqlwriter::straightJoin(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("straight_join");
	return true;
}

bool sqlwriter::left(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("left");
	return true;
}

bool sqlwriter::right(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("right");
	return true;
}

bool sqlwriter::outer(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("outer");
	return true;
}

bool sqlwriter::natural(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("natural");
	return true;
}

bool sqlwriter::join(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("join");
	return true;
}

bool sqlwriter::on(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on");
	return true;
}

bool sqlwriter::joinUsing(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("using");
	return true;
}

bool sqlwriter::groupBy(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("group by");
	return true;
}

bool sqlwriter::groupByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endGroupByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// write a comma if the next node is another group-by-item
	xmldomnode	*next=node->getNextTagSibling();
	if (!next->isNullNode() &&
		!charstring::compare(next->getName(),
					sqlparser::_group_by_item)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::withRollup(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("with rollup");
	return true;
}

bool sqlwriter::having(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("having");
	return true;
}

bool sqlwriter::orderBy(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("order by");
	return true;
}

bool sqlwriter::orderByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endOrderByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::asc(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("asc");
	return true;
}

bool sqlwriter::desc(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("desc");
	return true;
}

bool sqlwriter::limit(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("limit");
	return true;
}

bool sqlwriter::selectInto(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("into");
	return true;
}

bool sqlwriter::procedure(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("procedure");
	return true;
}

bool sqlwriter::forUpdate(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("for update");
	return true;
}
