// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

bool sqlrcontroller_svr::getQueryTreeCommand(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",1,"getting query tree");

	// get the tree as a string
	xmldomnode	*tree=(cursor->querytree)?
				cursor->querytree->getRootNode():NULL;
	stringbuffer	*xml=(tree)?tree->xml():NULL;
	const char	*xmlstring=(xml)?xml->getString():NULL;
	uint64_t	xmlstringlen=(xml)?xml->getStringLength():0;

	// send the tree
	clientsock->write(xmlstringlen);
	clientsock->write(xmlstring,xmlstringlen);
	flushWriteBuffer();

	// clean up
	delete xml;

	return true;
}
