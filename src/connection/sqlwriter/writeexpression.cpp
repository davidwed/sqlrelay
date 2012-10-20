// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <sqlparser.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

bool sqlwriter::expression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::intervalQualifier(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(node->getAttributeValue(sqlparser::_from));
	const char	*precision=
			node->getAttributeValue(sqlparser::_precision);
	if (precision) {
		leftParen(output);
		output->append(precision);
		rightParen(output);
	}
	output->append(" to ");
	output->append(node->getAttributeValue(sqlparser::_to));
	const char	*scale=
			node->getAttributeValue(sqlparser::_scale);
	if (scale) {
		leftParen(output);
		output->append(scale);
		rightParen(output);
	}
	return true;
}

bool sqlwriter::outerJoinOperator(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("(+)");
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

bool sqlwriter::columnReference(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::columnNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::columnNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::columnNameTable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::columnNameColumn(xmldomnode *node, stringbuffer *output) {
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
