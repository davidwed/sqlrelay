// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrfilterdeclarations.cpp"
	}
#endif

sqlrfilters::sqlrfilters(sqlrservercontroller *cont) : sqlrservermodules(cont) {
	pvt=NULL;
	setDebug(cont->getConfig()->getDebugFilters());
}

sqlrfilters::~sqlrfilters() {
}

bool sqlrfilters::load(domnode *parameters) {

	unload();

	// run through the module tags
	for (domnode *moduledata=parameters->getFirstTagChild();
				!moduledata->isNullNode();
				moduledata=moduledata->getNextTagSibling()) {

		// skip disabled modules
		if (isModuleDisabled(parameters)) {
			continue;
		}

		// load the module
		if (charstring::contains(
				moduledata->getAttributeValue("when"),
				"before")) {

			// before-filter
			loadModule(moduledata,&blist);

		} else {

			// after-filter
			loadModule(moduledata,&alist);
		}
	}
	return true;
}

void sqlrfilters::loadModule(domnode *parameters,
				singlylinkedlist< sqlrmoduleplugin * > *list) {

	// ignore non-filters
	if (charstring::compare(parameters->getName(),"filter")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading (%s) filter module: %s",
				(list==&blist)?"before":"after",
				module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the filter module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("filter_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load "
				"filter module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the filter itself
	stringbuffer	functionname;
	functionname.append("new_sqlrfilter_")->append(module);
	sqlrfilter *(*newFilter)(sqlrservercontroller *,
					domnode *)=
		(sqlrfilter *(*)(sqlrservercontroller *,
					domnode *))
				dl->getSymbol(functionname.getString());
	if (!newFilter) {
		stdoutput.printf("failed to load filter: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrfilter	*f=(*newFilter)(cont,parameters);

#else
	dynamiclib	*dl=NULL;
	sqlrfilter	*f;
	#include "sqlrfilterassignments.cpp"
	{
		f=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlrfp=new sqlrmoduleplugin;
	sqlrfp->m=f;
	sqlrfp->dl=dl;
	sqlrfp->module=module;
	list->append(sqlrfp);
}

bool sqlrfilters::runBeforeFilters(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrparser *sqlrp,
					const char *query,
					const char **err,
					int64_t *errn) {
	return run(sqlrcon,sqlrcur,sqlrp,query,err,errn,&blist);
}

bool sqlrfilters::runAfterFilters(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrparser *sqlrp,
					const char *query,
					const char **err,
					int64_t *errn) {
	return run(sqlrcon,sqlrcur,sqlrp,query,err,errn,&alist);
}

bool sqlrfilters::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrparser *sqlrp,
				const char *query,
				const char **err,
				int64_t *errn,
				singlylinkedlist< sqlrmoduleplugin * > *list) {

	if (!query) {
		return false;
	}

	xmldom	*tree=NULL;

	for (listnode< sqlrmoduleplugin * > *node=list->getFirst();
						node; node=node->getNext()) {

		debugWrite("running (%s) filter: %s...",
				(list==&blist)?"before":"after",
				node->getValue()->module);

		sqlrfilter	*f=(sqlrfilter *)node->getValue()->m;

		if (f->requiresTree()) {

			if (!sqlrp) {
				debugWrite("filter requires query tree "
						"but no parser available...");
				return true;
			}

			if (!tree) {
				if (!sqlrp->parse(query)) {
					debugWrite(
						"filter requires query tree "
						"but query failed to parse...");
					return true;
				}
				tree=sqlrp->getQueryTree();
				if (getDebug()) {
					debugWrite("query tree:");
					stringbuffer	b;
					tree->getRootNode()->write(&b,true);
					debugWrite(b.getString());
				}
			}

			if (!f->run(sqlrcon,sqlrcur,tree)) {
				f->getError(err,errn);
				return false;
			}

		} else {

			if (!f->run(sqlrcon,sqlrcur,query)) {
				f->getError(err,errn);
				return false;
			}
		}
	}
	return true;
}
