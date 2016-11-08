// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/xmldomnode.h>
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

class sqlrauthsprivate {
	friend class sqlrauths;
	private:
		const char	*_libexecdir;
		bool		_debug;

		singlylinkedlist< sqlrauthplugin * >	_llist;
};

sqlrauths::sqlrauths(sqlrpaths *sqlrpth, bool debug) {
	debugFunction();
	pvt=new sqlrauthsprivate;
	pvt->_libexecdir=sqlrpth->getLibExecDir();
	pvt->_debug=debug;
}

sqlrauths::~sqlrauths() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrauths::load(xmldomnode *parameters, sqlrpwdencs *sqlrpe) {
	debugFunction();

	unload();

	// run through each set of auths
	for (xmldomnode *auth=parameters->getFirstTagChild("auth");
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

void sqlrauths::loadAuth(xmldomnode *auth, sqlrpwdencs *sqlrpe) {
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
	modulename.append(pvt->_libexecdir);
	modulename.append(SQLR);
	modulename.append("auth_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load auth module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the password encryption itself
	stringbuffer	functionname;
	functionname.append("new_sqlrauth_")->append(module);
	sqlrauth *(*newAuth)(xmldomnode *, sqlrpwdencs *, bool)=
			(sqlrauth *(*)(xmldomnode *, sqlrpwdencs *, bool))
				dl->getSymbol(functionname.getString());
	if (!newAuth) {
		stdoutput.printf("failed to create auth: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrauth	*au=(*newAuth)(auth,sqlrpe,pvt->_debug);

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

const char *sqlrauths::auth(sqlrserverconnection *sqlrcon,
					sqlrcredentials *cred) {
	debugFunction();
	if (!cred) {
		return NULL;
	}
	for (singlylinkedlistnode< sqlrauthplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		const char	*autheduser=
				node->getValue()->au->auth(sqlrcon,cred);
		if (autheduser) {
			return autheduser;
		}
	}
	return NULL;
}
