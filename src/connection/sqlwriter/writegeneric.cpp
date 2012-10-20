// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

bool sqlwriter::tableNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::tableNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::tableNameTable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::name(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::type(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::endType(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::size(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlwriter::endSize(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::value(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::verbatim(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}
