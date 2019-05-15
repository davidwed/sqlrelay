// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrprotocoldeclarations.cpp"
	}
#endif

class sqlrprotocolplugin {
	public:
		sqlrprotocol	*pr;
		dynamiclib	*dl;
};

class sqlrprotocolsprivate {
	friend class sqlrprotocols;
	private:
		sqlrservercontroller	*_cont;

		dictionary< uint16_t , sqlrprotocolplugin * >	_protos;
};

sqlrprotocols::sqlrprotocols(sqlrservercontroller *cont) {
	debugFunction();
	pvt=new sqlrprotocolsprivate;
	pvt->_cont=cont;
}

sqlrprotocols::~sqlrprotocols() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrprotocols::load(domnode *parameters) {
	debugFunction();

	unload();

	// run through the listeners
	uint16_t	i=0;
	for (domnode *listener=parameters->getFirstTagChild();
			!listener->isNullNode();
			listener=listener->getNextTagSibling()) {

		debugPrintf("loading protocol ...\n");

		// load protocol
		loadProtocol(i,listener);

		i++;
	}
	return true;
}

void sqlrprotocols::unload() {
	debugFunction();
	for (linkedlistnode< dictionarynode< uint16_t, sqlrprotocolplugin * > *>
			*node=pvt->_protos.getList()->getFirst();
			node; node=node->getNext()) {
		sqlrprotocolplugin	*sqlrpp=node->getValue()->getValue();
		delete sqlrpp->pr;
		delete sqlrpp->dl;
		delete sqlrpp;
	}
	pvt->_protos.clear();
}

void sqlrprotocols::loadProtocol(uint16_t index, domnode *listener) {
	debugFunction();

	// ignore any non-listener entries
	if (charstring::compare(listener->getName(),"listener")) {
		return;
	}

	// get the protocol name
	const char	*module=listener->getAttributeValue("protocol");

	debugPrintf("loading protocol: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the protocol module
	stringbuffer	modulename;
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("protocol_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load protocol module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the protocol itself
	stringbuffer	functionname;
	functionname.append("new_sqlrprotocol_")->append(module);
	sqlrprotocol *(*newProtocol)(sqlrservercontroller *,
					sqlrprotocols *,
					domnode *)=
			(sqlrprotocol *(*)(sqlrservercontroller *,
						sqlrprotocols *,
						domnode *))
				dl->getSymbol(functionname.getString());
	if (!newProtocol) {
		stdoutput.printf("failed to load protocol: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrprotocol	*pr=(*newProtocol)(pvt->_cont,this,listener);

#else

	dynamiclib	*dl=NULL;
	sqlrprotocol	*pr;
	#include "sqlrprotocolassignments.cpp"
	{
		pr=NULL;
	}
#endif

	// add the plugin to the list
	sqlrprotocolplugin	*sqlrpp=new sqlrprotocolplugin;
	sqlrpp->pr=pr;
	sqlrpp->dl=dl;
	pvt->_protos.setValue(index,sqlrpp);
}

sqlrprotocol *sqlrprotocols::getProtocol(uint16_t index) {
	debugFunction();
	sqlrprotocolplugin	*pp=NULL;
	if (!pvt->_protos.getValue(index,&pp)) {
		return NULL;
	}
	return pp->pr;
}

void sqlrprotocols::endTransaction(bool commit) {
	for (linkedlistnode< dictionarynode< uint16_t, sqlrprotocolplugin * > *>
			*node=pvt->_protos.getList()->getFirst();
			node; node=node->getNext()) {
		node->getValue()->getValue()->pr->endTransaction(commit);
	}
}

void sqlrprotocols::endSession() {
	for (linkedlistnode< dictionarynode< uint16_t, sqlrprotocolplugin * > *>
			*node=pvt->_protos.getList()->getFirst();
			node; node=node->getNext()) {
		node->getValue()->getValue()->pr->endSession();
	}
}
