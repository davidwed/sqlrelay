// Copyright (c) 2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <debugprint.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrfilterdeclarations.cpp"
	}
#endif

sqlrfilters::sqlrfilters(sqlrpaths *sqlrpth, bool debug) {
	debugFunction();
	xmld=NULL;
	this->debug=debug;
	libexecdir=sqlrpth->getLibExecDir();
}

sqlrfilters::~sqlrfilters() {
	debugFunction();
	unloadFilters();
	delete xmld;
}

bool sqlrfilters::loadFilters(const char *filters) {
	debugFunction();

	unloadFilters();

	// create the parser
	delete xmld;
	xmld=new xmldom();

	// parse the filters
	if (!xmld->parseString(filters)) {
		return false;
	}

	// get the filters tag
	xmldomnode	*filtersnode=
			xmld->getRootNode()->getFirstTagChild("filters");
	if (filtersnode->isNullNode()) {
		return false;
	}

	// run through the filter list
	for (xmldomnode *filter=filtersnode->getFirstTagChild();
				!filter->isNullNode();
				filter=filter->getNextTagSibling()) {

		// load filter
		loadFilter(filter);
	}

	return true;
}

void sqlrfilters::unloadFilters() {
	debugFunction();
	for (singlylinkedlistnode< sqlrfilterplugin * > *node=
						tlist.getFirst();
						node; node=node->getNext()) {
		sqlrfilterplugin	*sqlrfp=node->getValue();
		delete sqlrfp->f;
		delete sqlrfp->dl;
		delete sqlrfp;
	}
	tlist.clear();
}

void sqlrfilters::loadFilter(xmldomnode *filter) {
	debugFunction();

	// ignore non-filters
	if (charstring::compare(filter->getName(),"filter")) {
		return;
	}

	// get the filter name
	const char	*module=filter->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=filter->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	if (debug) {
		stdoutput.printf("loading filter: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the filter module
	stringbuffer	modulename;
	modulename.append(libexecdir);
	modulename.append("sqlrfilter_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load "
				"filter module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the filter itself
	stringbuffer	functionname;
	functionname.append("new_sqlrfilter_")->append(module);
	sqlrfilter *(*newFilter)
		(sqlrfilters *, xmldomnode *, bool)=
		(sqlrfilter *(*)(sqlrfilters *, xmldomnode *, bool))
				dl->getSymbol(functionname.getString());
	if (!newFilter) {
		stdoutput.printf("failed to create filter: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrfilter	*f=(*newFilter)(this,filter,debug);

#else
	dynamiclib	*dl=NULL;
	sqlrfilter	*f;
	#include "sqlrfilterassignments.cpp"
	{
		f=NULL;
	}
#endif

	if (debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrfilterplugin	*sqlrfp=new sqlrfilterplugin;
	sqlrfp->f=f;
	sqlrfp->dl=dl;
	tlist.append(sqlrfp);
}

bool sqlrfilters::runFilters(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrparser *sqlrp,
					const char *query) {
	debugFunction();

	if (!query) {
		return false;
	}

	xmldom	*tree=NULL;

	for (singlylinkedlistnode< sqlrfilterplugin * > *node=
						tlist.getFirst();
						node; node=node->getNext()) {

		if (debug) {
			stdoutput.printf("\nrunning filter...\n\n");
		}

		sqlrfilter	*f=node->getValue()->f;

		if (f->usesTree()) {

			if (!tree) {
				if (!sqlrp->parse(query)) {
					return false;
				}
				tree=sqlrp->getTree();
				if (debug) {
					stdoutput.printf(
						"query tree:\n");
					tree->getRootNode()->print(&stdoutput);
					stdoutput.printf("\n");
				}
			}

			if (!f->run(sqlrcon,sqlrcur,tree)) {
				return false;
			}

		} else {

			if (!f->run(sqlrcon,sqlrcur,query)) {
				return false;
			}
		}
	}
	return true;
}

