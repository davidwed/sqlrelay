// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <rudiments/character.h>
#include <rudiments/xmldom.h>

bool sqlrcursor_svr::translateQuery() {

	// parse the query
	if (!conn->sqlp->parse(querybuffer)) {
		return false;
	}

	// get the parsed tree
	xmldom	*tree=conn->sqlp->detachTree();
	if (!tree) {
		return false;
	}

	// apply translation rules
	if (!conn->sqlt->applyRules(tree)) {
		delete tree;
		return false;
	}

	// write the query back out
	stringbuffer	translatedquery;
	if (!conn->sqlw->write(tree,&translatedquery)) {
		delete tree;
		return false;
	}
	delete tree;

	// copy translated query into query buffer
	if (translatedquery.getStringLength()>conn->maxquerysize) {
		// the translated query was too large
		return false;
	}
	charstring::copy(querybuffer,
			translatedquery.getString(),
			translatedquery.getStringLength());
	querylength=translatedquery.getStringLength();
	return true;
}
