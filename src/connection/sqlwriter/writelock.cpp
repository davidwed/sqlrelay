// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

bool sqlwriter::lockQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("lock");
	return true;
}

bool sqlwriter::inMode(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("in");
	return true;
}

bool sqlwriter::lockMode(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::mode(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("mode");
	return true;
}

bool sqlwriter::noWait(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("nowait");
	return true;
}
