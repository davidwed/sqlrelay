// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/domnode.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrnotificationdeclarations.cpp"
	}
#endif

class sqlrnotificationsprivate {
	friend class sqlrnotifications;
	private:
		const char	*_libexecdir;
};

sqlrnotifications::sqlrnotifications(sqlrpaths *sqlrpth) :
						sqlrservermodules(NULL) {
	pvt=new sqlrnotificationsprivate;
	pvt->_libexecdir=sqlrpth->getLibExecDir();
}

sqlrnotifications::~sqlrnotifications() {
	delete pvt;
}

void sqlrnotifications::loadModule(domnode *parameters) {

	// ignore non-notifications
	if (charstring::compare(parameters->getName(),"notification")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading notification module: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the notification module
	stringbuffer	modulename;
	modulename.append(pvt->_libexecdir);
	modulename.append(SQLR);
	modulename.append("notification_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf(
			"failed to load notification module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the notification itself
	stringbuffer	functionname;
	functionname.append("new_sqlrnotification_")->append(module);
	sqlrnotification *(*newNotification)(domnode *)=
		(sqlrnotification *(*)(domnode *))
				dl->getSymbol(functionname.getString());
	if (!newNotification) {
		stdoutput.printf("failed to load notification: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrnotification	*n=(*newNotification)(parameters);

#else

	dynamiclib	*dl=NULL;
	sqlrnotification	*n;
	#include "sqlrnotificationassignments.cpp"
	{
		n=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlrmp=new sqlrmoduleplugin;
	sqlrmp->m=n;
	sqlrmp->dl=dl;
	sqlrmp->module=module;
	blist.append(sqlrmp);
}

void sqlrnotifications::run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info) {

	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {

		debugWrite("running notification: %s...",
					node->getValue()->module);

		sqlrnotification	*n=
			(sqlrnotification *)node->getValue()->m;
		n->run(sqlrl,sqlrcon,sqlrcur,event,info);
	}
}
