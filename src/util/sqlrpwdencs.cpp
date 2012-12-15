// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrpwdencs.h>
#include <debugprint.h>

#include <rudiments/xmldomnode.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
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

	// ignore non-queries
	if (charstring::compare(pwdenc->getName(),"passwordencryption")) {
		return;
	}

	// get the password encryption name
	const char	*file=pwdenc->getAttributeValue("file");
	if (!charstring::length(file)) {
		return;
	}

	// get the password encryption id
	const char	*id=pwdenc->getAttributeValue("id");
	if (!charstring::length(id)) {
		return;
	}

	debugPrintf("loading password encryption: %s\n",file);

	// load the password encryption module
	stringbuffer	modulename;
	modulename.append(LIBDIR);
	modulename.append("/libsqlrelay_sqlrpwdenc_");
	modulename.append(file)->append(".so");
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		printf("failed to load password encryption module: %s\n",file);
		char	*error=dl->getError();
		printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the password encryption itself
	stringbuffer	functionname;
	functionname.append("new_")->append(file);
	sqlrpwdenc *(*newPasswordEncryption)(const char *)=
			(sqlrpwdenc *(*)(const char *))
				dl->getSymbol(functionname.getString());
	if (!newPasswordEncryption) {
		printf("failed to create password encryption: %s\n",file);
		char	*error=dl->getError();
		printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrpwdenc	*pe=(*newPasswordEncryption)(id);

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
