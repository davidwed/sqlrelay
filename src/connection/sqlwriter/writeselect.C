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
