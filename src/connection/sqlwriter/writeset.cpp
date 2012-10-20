// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

bool sqlwriter::setQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("set");
	return true;
}

bool sqlwriter::setSession(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("session");
	return true;
}

bool sqlwriter::setGlobal(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("global");
	return true;
}

bool sqlwriter::transaction(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("transaction ");
	return true;
}

bool sqlwriter::isolationLevel(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("isolation level ");
	outputValue(node,output);
	return true;
}
