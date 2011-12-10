// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <sqltranslatordebug.h>


bool sqlwriter::dropQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("drop");
	return true;
}

bool sqlwriter::ifExists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("if exists");
	return true;
}

bool sqlwriter::endTableNameListItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (!node->getNextSibling()->isNullNode()) {
		comma(output);
	}
	return true;
}

bool sqlwriter::restrictClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("restrict");
	return true;
}

bool sqlwriter::cascade(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("cascade");
	return true;
}
