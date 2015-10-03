// Copyright (c) 2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>

#define DEBUG_MESSAGES
#include <debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrconfigdeclarations.cpp"
	}
#endif

sqlrconfigs::sqlrconfigs(sqlrpaths *sqlrpth) {
	debugFunction();
	this->sqlrpth=sqlrpth;
	this->libexecdir=sqlrpth->getLibExecDir();
	cfg=NULL;
	dl=NULL;
}

sqlrconfigs::~sqlrconfigs() {
	debugFunction();
	delete cfg;
	delete dl;
}

sqlrconfig *sqlrconfigs::load(const char *urls, const char *id) {
	debugFunction();

	// separate the urls
	char		**url;
	uint64_t	urlcount;
	charstring::split(urls,",",true,&url,&urlcount);

	// for each url...
	for (const char * const *u=url; *u; u++) {

		// clean up from the previous iteration
		delete cfg;
		cfg=NULL;

		// parse out the protocol
	 	char	*protocol=NULL;
		const char	*colon=charstring::findFirst(*u,':');
		if (colon) {
			protocol=charstring::duplicate(*u,colon-*u);
		}

		// try to load a config plugin for that protocol
	 	if (protocol) {
	 		loadConfig(protocol);
			delete[] protocol;
	 	}

		// fall back to the xml plugin if that plugin failed to load
		if (!cfg) {
			loadConfig("xml");
		}

		// parse the configuration for the specified id
		if (cfg && cfg->parse(*u,id)) {
			break;
		}
	}

	return cfg;
}

void sqlrconfigs::loadConfig(const char *module) {
	debugFunction();

	debugPrintf("loading config: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the config module
	stringbuffer	modulename;
	modulename.append(libexecdir);
	modulename.append("sqlrconfig_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load config module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the config itself
	stringbuffer	functionname;
	functionname.append("new_sqlrconfig_")->append(module);
	sqlrconfig *(*newConfig)(sqlrpaths *)=
			(sqlrconfig *(*)(sqlrpaths *))
				dl->getSymbol(functionname.getString());
	if (!newConfig) {
		stdoutput.printf("failed to create config: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	cfg=(*newConfig)(sqlrpth);

#else
	sqlrconfig	*cfg;
	#include "sqlrconfigassignments.cpp"
	{
		cfg=NULL;
	}
#endif
}
