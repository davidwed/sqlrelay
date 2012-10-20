// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

bool sqlwriter::deleteQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("delete");
	return true;
}

bool sqlwriter::deleteFrom(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("from");
	return true;
}

bool sqlwriter::usingClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("using");
	return true;
}
