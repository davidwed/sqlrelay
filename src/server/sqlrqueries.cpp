// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/xmldomnode.h>
#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrquerydeclarations.cpp"
	}
#endif

class sqlrqueryplugin {
	public:
		sqlrquery	*qr;
		dynamiclib	*dl;
};

class sqlrqueriesprivate {
	friend class sqlrqueries;
	private:
		sqlrservercontroller	*_cont;

		singlylinkedlist< sqlrqueryplugin * >	_llist;
};

sqlrqueries::sqlrqueries(sqlrservercontroller *cont) {
	debugFunction();
	pvt=new sqlrqueriesprivate;
	pvt->_cont=cont;
}

sqlrqueries::~sqlrqueries() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrqueries::load(xmldomnode *parameters) {
	debugFunction();

	unload();

	// run through the query list
	for (xmldomnode *query=parameters->getFirstTagChild();
		!query->isNullNode(); query=query->getNextTagSibling()) {

		debugPrintf("loading query ...\n");

		// load query
		loadQuery(query);
	}
	return true;
}

void sqlrqueries::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrqueryplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		sqlrqueryplugin	*sqlrlp=node->getValue();
		delete sqlrlp->qr;
		delete sqlrlp->dl;
		delete sqlrlp;
	}
	pvt->_llist.clear();
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
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("query_");
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
	functionname.append("new_sqlrquery_")->append(module);
	sqlrquery *(*newQuery)(sqlrservercontroller *,
					xmldomnode *)=
			(sqlrquery *(*)(sqlrservercontroller *,
						xmldomnode *))
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
	sqlrquery	*qr=(*newQuery)(pvt->_cont,query);

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
	pvt->_llist.append(sqlrlp);
}

sqlrquerycursor *sqlrqueries::match(sqlrserverconnection *sqlrcon,
					const char *querystring,
					uint32_t querylength,
					uint16_t id) {
	debugFunction();
	for (singlylinkedlistnode< sqlrqueryplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		sqlrquery	*qr=node->getValue()->qr;
		if (qr->match(querystring,querylength)) {
			return qr->newCursor(sqlrcon,id);
		}
	}
	return NULL;
}

void sqlrqueries::endSession() {
	// nothing for now, maybe in the future
}
