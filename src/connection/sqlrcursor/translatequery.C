// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#define DEBUG_MESSAGES
#include <debugprint.h>
#include <rudiments/character.h>
#include <rudiments/xmldom.h>

bool sqlrcursor_svr::translateQuery() {

	debugPrintf("parsing: \"%s\"\n\n",querybuffer);

	// parse the query
	if (!conn->sqlp->parse(querybuffer)) {
		return false;
	}

	// get the parsed tree
	xmldom	*tree=conn->sqlp->detachTree();
	if (!tree) {
		return false;
	}

	debugPrintf("before translation:\n");
	debugPrintQueryTree(tree);
	debugPrintf("\n");

	// apply translation rules
	if (!conn->sqlt->applyRules(conn,this,tree)) {
		delete tree;
		return false;
	}

	debugPrintf("after translation:\n");
	debugPrintQueryTree(tree);
	debugPrintf("\n");

	// write the query back out
	stringbuffer	translatedquery;
	if (!conn->sqlw->write(conn,this,tree,&translatedquery)) {
		delete tree;
		return false;
	}
	delete tree;

	debugPrintf("translated: \"%s\"\n",translatedquery.getString());

	// copy the translated query into query buffer
	if (translatedquery.getStringLength()>conn->maxquerysize) {
		// the translated query was too large
		return false;
	}
	charstring::copy(querybuffer,
			translatedquery.getString(),
			translatedquery.getStringLength());
	querylength=translatedquery.getStringLength();
	querybuffer[querylength]='\0';
	return true;
}
