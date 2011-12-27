// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <debugprint.h>

bool sqlwriter::compliment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("~");
	return true;
}

bool sqlwriter::inverse(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("!");
	return true;
}

bool sqlwriter::plus(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("+");
	return true;
}

bool sqlwriter::minus(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("-");
	return true;
}

bool sqlwriter::times(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("*");
	return true;
}

bool sqlwriter::dividedBy(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("/");
	return true;
}

bool sqlwriter::bitwiseAnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("&");
	return true;
}

bool sqlwriter::bitwiseOr(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("|");
	return true;
}

bool sqlwriter::bitwiseXor(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("^");
	return true;
}

bool sqlwriter::logicalAnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("&&");
	return true;
}

bool sqlwriter::logicalOr(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("||");
	return true;
}
