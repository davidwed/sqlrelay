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
		#include "sqlrpwdencdeclarations.cpp"
	}
#endif

class sqlrpwdencplugin {
	public:
		sqlrpwdenc	*pe;
		dynamiclib	*dl;
};

class sqlrpwdencsprivate {
	friend class sqlrpwdencs;
	private:
		const char	*_libexecdir;
		bool		_debug;

		singlylinkedlist< sqlrpwdencplugin * >	_llist;
};

sqlrpwdencs::sqlrpwdencs(sqlrpaths *sqlrpth, bool debug) {
	debugFunction();
	pvt=new sqlrpwdencsprivate;
	pvt->_libexecdir=sqlrpth->getLibExecDir();
	pvt->_debug=debug;
}

sqlrpwdencs::~sqlrpwdencs() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrpwdencs::load(domnode *parameters) {
	debugFunction();

	unload();

	// run through the password encryption list
	for (domnode *pwdenc=parameters->getFirstTagChild();
		!pwdenc->isNullNode(); pwdenc=pwdenc->getNextTagSibling()) {

		debugPrintf("loading password encryption ...\n");

		// load password encryption
		loadPasswordEncryption(pwdenc);
	}
	return true;
}

void sqlrpwdencs::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrpwdencplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		sqlrpwdencplugin	*sqlrpe=node->getValue();
		delete sqlrpe->pe;
		delete sqlrpe->dl;
		delete sqlrpe;
	}
	pvt->_llist.clear();
}

void sqlrpwdencs::loadPasswordEncryption(domnode *pwdenc) {
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
	modulename.append(pvt->_libexecdir);
	modulename.append(SQLR);
	modulename.append("pwdenc_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load password "
				"encryption module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the password encryption itself
	stringbuffer	functionname;
	functionname.append("new_sqlrpwdenc_")->append(module);
	sqlrpwdenc *(*newPasswordEncryption)(domnode *, bool)=
			(sqlrpwdenc *(*)(domnode *, bool))
				dl->getSymbol(functionname.getString());
	if (!newPasswordEncryption) {
		stdoutput.printf("failed to load password "
				"encryption: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrpwdenc	*pe=(*newPasswordEncryption)(pwdenc,pvt->_debug);

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
	pvt->_llist.append(sqlrpe);
}

sqlrpwdenc *sqlrpwdencs::getPasswordEncryptionById(const char *id) {
	for (singlylinkedlistnode< sqlrpwdencplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		sqlrpwdenc	*pe=node->getValue()->pe;
		if (!charstring::compare(pe->getId(),id)) {
			return pe;
		}
	}
	return NULL;
}
