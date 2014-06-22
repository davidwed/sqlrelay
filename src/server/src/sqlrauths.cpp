// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrauths.h>

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

bool sqlrauths::loadAuthenticators(const char *auths,
					sqlrpwdencs *sqlrpe) {
	debugFunction();

	unloadAuthenticators();

	// create the parser
	delete xmld;
	xmld=new xmldom();

	// parse the auths
	if (!xmld->parseString(auths)) {
		return false;
	}

	// get the auths tag
	xmldomnode	*authsnode=
		xmld->getRootNode()->getFirstTagChild("authentications");
	if (authsnode->isNullNode()) {
		return false;
	}

	// run through each set of auths
	for (xmldomnode *auth=authsnode->getFirstTagChild("authentication");
			!auth->isNullNode();
			auth=auth->getNextTagSibling("authentication")) {

		debugPrintf("loading authenticator ...\n");

		// load password encryption
		loadAuthenticator(auth,sqlrpe);
	}
	return true;
}

void sqlrauths::unloadAuthenticators() {
	debugFunction();
	for (linkedlistnode< sqlrauthplugin * > *node=llist.getFirst();
						node; node=node->getNext()) {
		sqlrauthplugin	*sqlrap=node->getValue();
		delete sqlrap->au;
		delete sqlrap->dl;
		delete sqlrap;
	}
	llist.clear();
}

void sqlrauths::loadAuthenticator(xmldomnode *auth,
					sqlrpwdencs *sqlrpe) {
	debugFunction();

	// get the authenticator name
	const char	*module=auth->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=auth->getAttributeValue("file");
		if (!charstring::length(module)) {
			// fall back to default if no module is specified
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
	sqlrauth	*au=(*newAuthenticator)(auth,sqlrpe);

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

bool sqlrauths::authenticate(const char *user, const char *password) {
	debugFunction();
	for (linkedlistnode< sqlrauthplugin * > *node=llist.getFirst();
						node; node=node->getNext()) {
		if (node->getValue()->au->authenticate(user,password)) {
			return true;
		}
	}
	return false;
}
