// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrfilterdeclarations.cpp"
	}
#endif

class sqlrfilterplugin {
	public:
		sqlrfilter	*f;
		dynamiclib	*dl;
};

class sqlrfiltersprivate {
	friend class sqlrfilters;
	private:
		sqlrservercontroller	*_cont;

		bool	_debug;

		singlylinkedlist< sqlrfilterplugin * >	_beforefilters;
		singlylinkedlist< sqlrfilterplugin * >	_afterfilters;
};

sqlrfilters::sqlrfilters(sqlrservercontroller *cont) {
	debugFunction();
	pvt=new sqlrfiltersprivate;
	pvt->_cont=cont;
	pvt->_debug=cont->getConfig()->getDebugFilters();
}

sqlrfilters::~sqlrfilters() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrfilters::load(domnode *parameters) {
	debugFunction();

	unload();

	// run through the filter list
	for (domnode *filter=parameters->getFirstTagChild();
				!filter->isNullNode();
				filter=filter->getNextTagSibling()) {

		if (charstring::contains(
				filter->getAttributeValue("when"),
				"before")) {

			// add filter to before list
			if (pvt->_debug) {
				stdoutput.printf("loading filter before ...\n");
			}
			loadFilter(filter,&pvt->_beforefilters);

		} else {

			// add filter to after list
			if (pvt->_debug) {
				stdoutput.printf("loading filter after ...\n");
			}
			loadFilter(filter,&pvt->_afterfilters);
		}
	}
	return true;
}

void sqlrfilters::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrfilterplugin * > *node=
						pvt->_beforefilters.getFirst();
						node; node=node->getNext()) {
		sqlrfilterplugin	*sqlrfp=node->getValue();
		delete sqlrfp->f;
		delete sqlrfp->dl;
		delete sqlrfp;
	}
	pvt->_beforefilters.clear();
	for (singlylinkedlistnode< sqlrfilterplugin * > *node=
						pvt->_afterfilters.getFirst();
						node; node=node->getNext()) {
		sqlrfilterplugin	*sqlrfp=node->getValue();
		delete sqlrfp->f;
		delete sqlrfp->dl;
		delete sqlrfp;
	}
	pvt->_afterfilters.clear();
}

void sqlrfilters::loadFilter(domnode *filter, 
				singlylinkedlist< sqlrfilterplugin * > *list) {
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

	if (pvt->_debug) {
		stdoutput.printf("loading filter: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the filter module
	stringbuffer	modulename;
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
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
					sqlrfilters *,
					domnode *)=
		(sqlrfilter *(*)(sqlrservercontroller *,
					sqlrfilters *,
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
	sqlrfilter	*f=(*newFilter)(pvt->_cont,this,filter);

#else
	dynamiclib	*dl=NULL;
	sqlrfilter	*f;
	#include "sqlrfilterassignments.cpp"
	{
		f=NULL;
	}
#endif

	if (pvt->_debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrfilterplugin	*sqlrfp=new sqlrfilterplugin;
	sqlrfp->f=f;
	sqlrfp->dl=dl;
	list->append(sqlrfp);
}

bool sqlrfilters::runBeforeFilters(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrparser *sqlrp,
					const char *query,
					const char **err,
					int64_t *errn) {
	debugFunction();
	return run(sqlrcon,sqlrcur,sqlrp,query,err,errn,&pvt->_beforefilters);
}

bool sqlrfilters::runAfterFilters(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrparser *sqlrp,
					const char *query,
					const char **err,
					int64_t *errn) {
	debugFunction();
	return run(sqlrcon,sqlrcur,sqlrp,query,err,errn,&pvt->_afterfilters);
}

bool sqlrfilters::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrparser *sqlrp,
				const char *query,
				const char **err,
				int64_t *errn,
				singlylinkedlist< sqlrfilterplugin * > *list) {
	debugFunction();

	if (!query) {
		return false;
	}

	xmldom	*tree=NULL;

	for (singlylinkedlistnode< sqlrfilterplugin * > *node=list->getFirst();
						node; node=node->getNext()) {

		if (pvt->_debug) {
			stdoutput.printf("\nrunning filter...\n\n");
		}

		sqlrfilter	*f=node->getValue()->f;

		if (f->usesTree()) {

			if (!sqlrp) {
				if (pvt->_debug) {
					stdoutput.printf("\nfilter "
							"requires query tree "
							"but no parser "
							"available...\n\n");
				}
				return true;
			}

			if (!tree) {
				if (!sqlrp->parse(query)) {
					if (pvt->_debug) {
						stdoutput.printf("\nfilter "
							"requires query tree "
							"but query failed to "
							"parse ...\n\n");
					}
					return true;
				}
				tree=sqlrp->getTree();
				if (pvt->_debug) {
					stdoutput.printf(
						"query tree:\n");
					tree->getRootNode()->
						write(&stdoutput,true);
					stdoutput.printf("\n");
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

void sqlrfilters::endTransaction(bool commit) {
	for (singlylinkedlistnode< sqlrfilterplugin * > *node=
						pvt->_beforefilters.getFirst();
						node; node=node->getNext()) {
		node->getValue()->f->endTransaction(commit);
	}
	for (singlylinkedlistnode< sqlrfilterplugin * > *node=
						pvt->_afterfilters.getFirst();
						node; node=node->getNext()) {
		node->getValue()->f->endTransaction(commit);
	}
}

void sqlrfilters::endSession() {
	for (singlylinkedlistnode< sqlrfilterplugin * > *node=
						pvt->_beforefilters.getFirst();
						node; node=node->getNext()) {
		node->getValue()->f->endSession();
	}
	for (singlylinkedlistnode< sqlrfilterplugin * > *node=
						pvt->_afterfilters.getFirst();
						node; node=node->getNext()) {
		node->getValue()->f->endSession();
	}
}
