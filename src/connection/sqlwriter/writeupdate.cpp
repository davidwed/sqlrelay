// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

bool sqlwriter::updateQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("update");
	return true;
}

bool sqlwriter::updateSet(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("set");
	return true;
}

bool sqlwriter::assignment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endAssignment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		output->append(",");
	}
	return true;
}

bool sqlwriter::equals(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("=");
	return true;
}
