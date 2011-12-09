// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <sqltranslatordebug.h>


bool sqlwriter::dropQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("drop");
	return true;
}
