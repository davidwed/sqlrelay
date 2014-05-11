// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <sqlrauths.h>

#include <debugprint.h>

#include <rudiments/xmldomnode.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrauthdeclarations.cpp"
	}
#endif

sqlrauths::sqlrauths() {
	debugFunction();
	xmld=NULL;
}

sqlrauths::~sqlrauths() {
	debugFunction();
	unloadAuthenticators();
	delete xmld;
}

bool sqlrauths::loadAuthenticators(const char *instance,
					sqlrpwdencs *sqlrpe) {
	debugFunction();

	unloadAuthenticators();

	// create the parser
	delete xmld;
	xmld=new xmldom();

	// parse the instance
	if (!xmld->parseString(instance)) {
		return false;
	}

	// get the instance tag
	xmldomnode	*instances=
		xmld->getRootNode()->getFirstTagChild("instances");
	if (instances->isNullNode()) {
		return false;
	}

	// run through each set of users
	for (xmldomnode *users=instances->getFirstTagChild("users");
		!users->isNullNode(); users=users->getNextTagSibling("users")) {

		debugPrintf("loading authenticator ...\n");

		// load password encryption
		loadAuthenticator(users,sqlrpe);
	}
	return true;
}

void sqlrauths::unloadAuthenticators() {
	debugFunction();
	for (linkedlistnode< sqlrauthplugin * > *node=
				llist.getFirstNode();
					node; node=node->getNext()) {
		sqlrauthplugin	*sqlrap=node->getValue();
		delete sqlrap->au;
		delete sqlrap->dl;
		delete sqlrap;
	}
	llist.clear();
}

void sqlrauths::loadAuthenticator(xmldomnode *users,
					sqlrpwdencs *sqlrpe) {
	debugFunction();

	// get the authenticator name
	const char	*module=users->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=users->getAttributeValue("file");
		if (!charstring::length(module)) {
			// if that fails, fall back to "default"
			// to load the default module
			module="default";
		}
	}

	debugPrintf("loading authenticator: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the password encryption module
	stringbuffer	modulename;
	modulename.append(LIBEXECDIR);
	modulename.append("/sqlrauth_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load authentication "
					"module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the password encryption itself
	stringbuffer	functionname;
	functionname.append("new_")->append(module);
	sqlrauth *(*newAuthenticator)(xmldomnode *,
					sqlrpwdencs *)=
			(sqlrauth *(*)(xmldomnode *,
					sqlrpwdencs *))
				dl->getSymbol(functionname.getString());
	if (!newAuthenticator) {
		stdoutput.printf("failed to create authenticator: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrauth	*au=(*newAuthenticator)(users,sqlrpe);

#else

	dynamiclib	*dl=NULL;
	sqlrauth	*au;
	#include "sqlrauthassignments.cpp"
	{
		au=NULL;
	}
#endif

	// add the plugin to the list
	sqlrauthplugin	*sqlrap=new sqlrauthplugin;
	sqlrap->au=au;
	sqlrap->dl=dl;
	llist.append(sqlrap);
}
