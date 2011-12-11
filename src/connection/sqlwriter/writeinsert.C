// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <sqltranslatordebug.h>

bool sqlwriter::insertQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("insert");
	return true;
}

bool sqlwriter::insertInto(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("into");
	return true;
}
