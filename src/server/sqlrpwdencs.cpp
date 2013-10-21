// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrpwdencs.h>

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

sqlrpwdencs::sqlrpwdencs() {
	debugFunction();
	xmld=NULL;
}

sqlrpwdencs::~sqlrpwdencs() {
	debugFunction();
	unloadPasswordEncryptions();
	delete xmld;
}

bool sqlrpwdencs::loadPasswordEncryptions(const char *pwdencs) {
	debugFunction();

	unloadPasswordEncryptions();

	// create the parser
	delete xmld;
	xmld=new xmldom();

	// parse the password encryptions
	if (!xmld->parseString(pwdencs)) {
		return false;
	}

	// get the password encryptions tag
	xmldomnode	*pwdencsnode=
		xmld->getRootNode()->getFirstTagChild("passwordencryptions");
	if (pwdencsnode->isNullNode()) {
		return false;
	}

	// run through the password encryption list
	for (xmldomnode *pwdenc=pwdencsnode->getFirstTagChild();
		!pwdenc->isNullNode(); pwdenc=pwdenc->getNextTagSibling()) {

		debugPrintf("loading password encryption ...\n");

		// load password encryption
		loadPasswordEncryption(pwdenc);
	}
	return true;
}

void sqlrpwdencs::unloadPasswordEncryptions() {
	debugFunction();
	for (linkedlistnode< sqlrpwdencplugin * > *node=
				llist.getFirstNode();
					node; node=node->getNext()) {
		sqlrpwdencplugin	*sqlrlp=node->getData();
		delete sqlrlp->pe;
		delete sqlrlp->dl;
		delete sqlrlp;
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
	modulename.append(LIBEXECDIR);
	modulename.append("/sqlrpwdenc_");
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
	functionname.append("new_")->append(module);
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
	sqlrpwdencplugin	*sqlrlp=new sqlrpwdencplugin;
	sqlrlp->pe=pe;
	sqlrlp->dl=dl;
	llist.append(sqlrlp);
}

sqlrpwdenc *sqlrpwdencs::getPasswordEncryptionById(const char *id) {
	for (linkedlistnode< sqlrpwdencplugin * > *node=llist.getFirstNode();
						node; node=node->getNext()) {
		sqlrpwdenc	*pe=node->getData()->pe;
		if (!charstring::compare(pe->getId(),id)) {
			return pe;
		}
	}
	return NULL;
}
