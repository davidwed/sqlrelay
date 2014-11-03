// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrqueries.h>
#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>
#include <debugprint.h>

#include <rudiments/xmldomnode.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrquerydeclarations.cpp"
	}
#endif

sqlrqueries::sqlrqueries() {
	debugFunction();
	xmld=NULL;
}

sqlrqueries::~sqlrqueries() {
	debugFunction();
	unloadQueries();
	delete xmld;
}

bool sqlrqueries::loadQueries(const char *queries) {
	debugFunction();

	unloadQueries();

	// create the parser
	delete xmld;
	xmld=new xmldom();

	// parse the queries
	if (!xmld->parseString(queries)) {
		return false;
	}

	// get the queries tag
	xmldomnode	*queriesnode=
			xmld->getRootNode()->getFirstTagChild("queries");
	if (queriesnode->isNullNode()) {
		return false;
	}

	// run through the query list
	for (xmldomnode *query=queriesnode->getFirstTagChild();
		!query->isNullNode(); query=query->getNextTagSibling()) {

		debugPrintf("loading query ...\n");

		// load query
		loadQuery(query);
	}
	return true;
}

void sqlrqueries::unloadQueries() {
	debugFunction();
	for (singlylinkedlistnode< sqlrqueryplugin * > *node=llist.getFirst();
						node; node=node->getNext()) {
		sqlrqueryplugin	*sqlrlp=node->getValue();
		delete sqlrlp->qr;
		delete sqlrlp->dl;
		delete sqlrlp;
	}
	llist.clear();
}

void sqlrqueries::loadQuery(xmldomnode *query) {

	debugFunction();

	// ignore non-queries
	if (charstring::compare(query->getName(),"query")) {
		return;
	}

	// get the query name
	const char	*module=query->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=query->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	debugPrintf("loading query: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the query module
	stringbuffer	modulename;
	modulename.append(LIBEXECDIR);
	modulename.append("/sqlrquery_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load query module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the query itself
	stringbuffer	functionname;
	functionname.append("new_")->append(module);
	sqlrquery *(*newQuery)(xmldomnode *)=
			(sqlrquery *(*)(xmldomnode *))
				dl->getSymbol(functionname.getString());
	if (!newQuery) {
		stdoutput.printf("failed to create query: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrquery	*qr=(*newQuery)(query);

#else

	dynamiclib	*dl=NULL;
	sqlrquery	*qr;
	#include "sqlrqueryassignments.cpp"
	{
		qr=NULL;
	}
#endif

	// add the plugin to the list
	sqlrqueryplugin	*sqlrlp=new sqlrqueryplugin;
	sqlrlp->qr=qr;
	sqlrlp->dl=dl;
	llist.append(sqlrlp);
}

sqlrquerycursor *sqlrqueries::match(sqlrconnection_svr *sqlrcon,
					const char *querystring,
					uint32_t querylength,
					uint16_t id) {
	debugFunction();
	for (singlylinkedlistnode< sqlrqueryplugin * > *node=llist.getFirst();
						node; node=node->getNext()) {
		sqlrquery	*qr=node->getValue()->qr;
		if (qr->match(querystring,querylength)) {
			return qr->newCursor(sqlrcon,id);
		}
	}
	return NULL;
}
