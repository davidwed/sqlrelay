// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

bool sqlwriter::where(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("where");
	return true;
}

bool sqlwriter::andClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("and");
	return true;
}

bool sqlwriter::orClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("or");
	return true;
}

bool sqlwriter::comparison(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::notClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("not");
	return true;
}

bool sqlwriter::group(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("(");
	return true;
}

bool sqlwriter::endGroup(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlwriter::between(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("between");
	return true;
}

bool sqlwriter::in(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("in ");
	leftParen(output);
	return true;
}

bool sqlwriter::endIn(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::inSetItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endInSetItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::exists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("exists ");
	leftParen(output);
	return true;
}

bool sqlwriter::endExists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::is(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("is");
	return true;
}

bool sqlwriter::like(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("like");
	return true;
}

bool sqlwriter::matches(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("matches");
	return true;
}

bool sqlwriter::nullSafeEquals(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("<=>");
	return true;
}

bool sqlwriter::notEquals(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("!=");
	return true;
}

bool sqlwriter::lessThan(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("<");
	return true;
}

bool sqlwriter::greaterThan(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(">");
	return true;
}

bool sqlwriter::lessThanOrEqualTo(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("<=");
	return true;
}

bool sqlwriter::greaterThanOrEqualTo(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(">=");
	return true;
}

bool sqlwriter::escape(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("escape");
	return true;
}
