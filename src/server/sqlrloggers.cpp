// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/domnode.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrloggerdeclarations.cpp"
	}
#endif

class sqlrloggersprivate {
	friend class sqlrloggers;
	private:
		const char	*_libexecdir;
};

sqlrloggers::sqlrloggers(sqlrpaths *sqlrpth) : sqlrservermodules(NULL) {
	pvt=new sqlrloggersprivate;
	pvt->_libexecdir=sqlrpth->getLibExecDir();
}

sqlrloggers::~sqlrloggers() {
	delete pvt;
}

void sqlrloggers::loadModule(domnode *parameters) {

	// ignore non-loggers
	if (charstring::compare(parameters->getName(),"logger")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading logger module: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the logger module
	stringbuffer	modulename;
	modulename.append(pvt->_libexecdir);
	modulename.append(SQLR);
	modulename.append("logger_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load logger module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the logger itself
	stringbuffer	functionname;
	functionname.append("new_sqlrlogger_")->append(module);
	sqlrlogger *(*newLogger)(domnode *)=
			(sqlrlogger *(*)(domnode *))
				dl->getSymbol(functionname.getString());
	if (!newLogger) {
		stdoutput.printf("failed to load logger: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}
	sqlrlogger	*lg=(*newLogger)(parameters);

#else

	dynamiclib	*dl=NULL;
	sqlrlogger	*lg;
	#include "sqlrloggerassignments.cpp"
	{
		lg=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlrmp=new sqlrmoduleplugin;
	sqlrmp->m=lg;
	sqlrmp->dl=dl;
	sqlrmp->module=module;
	blist.append(sqlrmp);
}

void sqlrloggers::init(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon) {
	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {
		sqlrlogger	*lg=(sqlrlogger *)node->getValue()->m;
		lg->init(sqlrl,sqlrcon);
	}
}

void sqlrloggers::run(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrloglevel_t level,
				sqlrevent_t event,
				const char *info) {

	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {

		debugWrite("running logger: %s...",node->getValue()->module);

		sqlrlogger	*lg=(sqlrlogger *)node->getValue()->m;
		lg->run(sqlrl,sqlrcon,sqlrcur,level,event,info);
	}
}
