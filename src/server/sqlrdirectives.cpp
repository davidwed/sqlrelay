// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES
#include <rudiments/debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrdirectivedeclarations.cpp"
	}
#endif

class sqlrdirectiveplugin {
	public:
		sqlrdirective	*dr;
		dynamiclib	*dl;
		const char	*module;
};

class sqlrdirectivesprivate {
	friend class sqlrdirectives;
	private:
		sqlrservercontroller	*_cont;
		
		bool	_debug;

		singlylinkedlist< sqlrdirectiveplugin * >	_dlist;
};

sqlrdirectives::sqlrdirectives(sqlrservercontroller *cont) {
	debugFunction();
	pvt=new sqlrdirectivesprivate;
	pvt->_cont=cont;
	pvt->_debug=cont->getConfig()->getDebugDirectives();
}

sqlrdirectives::~sqlrdirectives() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrdirectives::load(domnode *parameters) {
	debugFunction();

	unload();

	// run through the directive list
	for (domnode *directive=parameters->getFirstTagChild();
				!directive->isNullNode();
				directive=directive->getNextTagSibling()) {

		// load directive
		loadDirective(directive);
	}

	return true;
}

void sqlrdirectives::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrdirectiveplugin * > *node=
						pvt->_dlist.getFirst();
						node; node=node->getNext()) {
		sqlrdirectiveplugin	*sqlt=node->getValue();
		delete sqlt->dr;
		delete sqlt->dl;
		delete sqlt;
	}
	pvt->_dlist.clear();
}

void sqlrdirectives::loadDirective(domnode *directive) {
	debugFunction();

	// ignore non-directives
	if (charstring::compare(directive->getName(),"directive")) {
		return;
	}

	// get the directive name
	const char	*module=directive->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=directive->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	if (pvt->_debug) {
		stdoutput.printf("loading directive module: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the directive module
	stringbuffer	modulename;
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("directive_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load "
				"directive module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the directive itself
	stringbuffer	functionname;
	functionname.append("new_sqlrdirective_")->append(module);
	sqlrdirective *(*newDirective)(sqlrservercontroller *,
						sqlrdirectives *,
						domnode *)=
		(sqlrdirective *(*)(sqlrservercontroller *,
						sqlrdirectives *,
						domnode *))
				dl->getSymbol(functionname.getString());
	if (!newDirective) {
		stdoutput.printf("failed to load directive: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrdirective	*dr=(*newDirective)(pvt->_cont,this,directive);

#else
	dynamiclib	*dl=NULL;
	sqlrdirective	*dr;
	#include "sqlrdirectiveassignments.cpp"
	{
		dr=NULL;
	}
#endif

	if (pvt->_debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrdirectiveplugin	*sqltp=new sqlrdirectiveplugin;
	sqltp->dr=dr;
	sqltp->dl=dl;
	sqltp->module=module;
	pvt->_dlist.append(sqltp);
}

bool sqlrdirectives::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query) {
	debugFunction();

	if (!query) {
		return false;
	}

	for (singlylinkedlistnode< sqlrdirectiveplugin * > *node=
						pvt->_dlist.getFirst();
						node; node=node->getNext()) {

		if (pvt->_debug) {
			stdoutput.printf("\nrunning directive:  %s...\n\n",
						node->getValue()->module);
		}

		sqlrdirective	*dr=node->getValue()->dr;

		bool	success=dr->run(sqlrcon,sqlrcur,query);
		if (!success) {
			return false;
		}
	}
	return true;
}
