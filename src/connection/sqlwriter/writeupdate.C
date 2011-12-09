// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <sqltranslatordebug.h>

bool sqlwriter::updateQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("update");
	return true;
}
