// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/domnode.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrauthdeclarations.cpp"
	}
#endif

class sqlrauthplugin {
	public:
		sqlrauth	*au;
		dynamiclib	*dl;
};

class sqlrauthsprivate {
	friend class sqlrauths;
	private:
		sqlrservercontroller	*_cont;

		singlylinkedlist< sqlrauthplugin * >	_llist;
};

sqlrauths::sqlrauths(sqlrservercontroller *cont) {
	debugFunction();
	pvt=new sqlrauthsprivate;
	pvt->_cont=cont;
}

sqlrauths::~sqlrauths() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrauths::load(domnode *parameters, sqlrpwdencs *sqlrpe) {
	debugFunction();

	unload();

	// run through each set of auths
	for (domnode *auth=parameters->getFirstTagChild("auth");
			!auth->isNullNode();
			auth=auth->getNextTagSibling("auth")) {

		debugPrintf("loading auth ...\n");

		// load password encryption
		loadAuth(auth,sqlrpe);
	}
	return true;
}

void sqlrauths::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrauthplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		sqlrauthplugin	*sqlrap=node->getValue();
		delete sqlrap->au;
		delete sqlrap->dl;
		delete sqlrap;
	}
	pvt->_llist.clear();
}

void sqlrauths::loadAuth(domnode *auth, sqlrpwdencs *sqlrpe) {
	debugFunction();

	// get the auth name
	const char	*module=auth->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=auth->getAttributeValue("file");
		if (!charstring::length(module)) {
			// fall back to default if no module is specified
			module="default";
		}
	}

	debugPrintf("loading auth: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the password encryption module
	stringbuffer	modulename;
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("auth_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load auth module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the password encryption itself
	stringbuffer	functionname;
	functionname.append("new_sqlrauth_")->append(module);
	sqlrauth *(*newAuth)(sqlrservercontroller *,
					sqlrauths *,
					sqlrpwdencs *,
					domnode *)=
			(sqlrauth *(*)(sqlrservercontroller *,
					sqlrauths *,
					sqlrpwdencs *,
					domnode *))
				dl->getSymbol(functionname.getString());
	if (!newAuth) {
		stdoutput.printf("failed to load auth: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrauth	*au=(*newAuth)(pvt->_cont,this,sqlrpe,auth);

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
	pvt->_llist.append(sqlrap);
}

const char *sqlrauths::auth(sqlrcredentials *cred) {
	debugFunction();
	if (!cred) {
		return NULL;
	}
	for (singlylinkedlistnode< sqlrauthplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		const char	*autheduser=node->getValue()->au->auth(cred);
		if (autheduser) {
			return autheduser;
		}
	}
	return NULL;
}

void sqlrauths::endSession() {
	// nothing for now, maybe in the future
}
