// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <debugprint.h>

bool sqlwriter::expression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

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

bool sqlwriter::negative(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("-");
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

bool sqlwriter::modulo(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("%");
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

bool sqlwriter::number(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::stringLiteral(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::bindVariable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::function(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::parameters(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlwriter::endParameters(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::parameter(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endParameter(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}
