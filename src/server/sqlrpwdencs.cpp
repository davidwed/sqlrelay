// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <debugprint.h>

#include <rudiments/xmldomnode.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrpwdencdeclarations.cpp"
	}
#endif

sqlrpwdencs::sqlrpwdencs(sqlrpaths *sqlrpth) {
	debugFunction();
	libexecdir=sqlrpth->getLibExecDir();
}

sqlrpwdencs::~sqlrpwdencs() {
	debugFunction();
	unloadPasswordEncryptions();
}

bool sqlrpwdencs::loadPasswordEncryptions(xmldomnode *parameters) {
	debugFunction();

	unloadPasswordEncryptions();

	// run through the password encryption list
	for (xmldomnode *pwdenc=parameters->getFirstTagChild();
		!pwdenc->isNullNode(); pwdenc=pwdenc->getNextTagSibling()) {

		debugPrintf("loading password encryption ...\n");

		// load password encryption
		loadPasswordEncryption(pwdenc);
	}
	return true;
}

void sqlrpwdencs::unloadPasswordEncryptions() {
	debugFunction();
	for (singlylinkedlistnode< sqlrpwdencplugin * > *node=llist.getFirst();
						node; node=node->getNext()) {
		sqlrpwdencplugin	*sqlrpe=node->getValue();
		delete sqlrpe->pe;
		delete sqlrpe->dl;
		delete sqlrpe;
	}
	llist.clear();
}

void sqlrpwdencs::loadPasswordEncryption(xmldomnode *pwdenc) {
	debugFunction();

	// ignore non-pssword encryptions
	if (charstring::compare(pwdenc->getName(),"passwordencryption")) {
		return;
	}

	// get the password encryption name
	const char	*module=pwdenc->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=pwdenc->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	// make sure it has an id
	if (!charstring::length(pwdenc->getAttributeValue("id"))) {
		return;
	}

	debugPrintf("loading password encryption: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the password encryption module
	stringbuffer	modulename;
	modulename.append(libexecdir);
	modulename.append(SQLR);
	modulename.append("pwdenc_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load password "
				"encryption module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the password encryption itself
	stringbuffer	functionname;
	functionname.append("new_sqlrpwdenc_")->append(module);
	sqlrpwdenc *(*newPasswordEncryption)(xmldomnode *)=
			(sqlrpwdenc *(*)(xmldomnode *))
				dl->getSymbol(functionname.getString());
	if (!newPasswordEncryption) {
		stdoutput.printf("failed to create password "
				"encryption: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrpwdenc	*pe=(*newPasswordEncryption)(pwdenc);

#else

	dynamiclib	*dl=NULL;
	sqlrpwdenc	*pe;
	#include "sqlrpwdencassignments.cpp"
	{
		pe=NULL;
	}
#endif

	// add the plugin to the list
	sqlrpwdencplugin	*sqlrpe=new sqlrpwdencplugin;
	sqlrpe->pe=pe;
	sqlrpe->dl=dl;
	llist.append(sqlrpe);
}

sqlrpwdenc *sqlrpwdencs::getPasswordEncryptionById(const char *id) {
	for (singlylinkedlistnode< sqlrpwdencplugin * > *node=llist.getFirst();
						node; node=node->getNext()) {
		sqlrpwdenc	*pe=node->getValue()->pe;
		if (!charstring::compare(pe->getId(),id)) {
			return pe;
		}
	}
	return NULL;
}
