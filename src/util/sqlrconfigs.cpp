// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>

//#define DEBUG_MESSAGES
#include <rudiments/debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrconfigdeclarations.cpp"
	}
#endif

sqlrconfigs::sqlrconfigs(sqlrpaths *sqlrpth) {
	debugFunction();
	this->libexecdir=sqlrpth->getLibExecDir();
	cfg=NULL;
	dl=NULL;
}

sqlrconfigs::~sqlrconfigs() {
	debugFunction();
	delete cfg;
	delete dl;
}

void sqlrconfigs::getEnabledIds(const char *urls,
					linkedlist< char * > *idlist) {
	debugFunction();

	// separate the urls
	char		**url;
	uint64_t	urlcount;
	charstring::split(urls,",",true,&url,&urlcount);

	// for each url...
	for (uint64_t i=0; i<urlcount; i++) {

		const char	*u=url[i];

		// parse out the protocol
	 	char		*protocol=NULL;
		const char	*colon=charstring::findFirst(u,':');
		if (colon) {
			protocol=charstring::duplicate(u,colon-u);
		}

		// try to load a config plugin for that protocol
	 	if (protocol) {
	 		loadConfig(protocol);
			delete[] protocol;
	 	}

		// fall back to the xml plugin if that plugin failed to load
		if (!cfg) {
			loadConfig("xmldom");
		}

		// get the enabled ids for the specified url
		if (cfg) {
			cfg->getEnabledIds(u,idlist);
		}

		// clean up
		delete cfg;
		cfg=NULL;
		delete dl;
		dl=NULL;
	}

	// clean up
	for (uint64_t i=0; i<urlcount; i++) {
		delete[] url[i];
	}
	delete[] url;
}

sqlrconfig *sqlrconfigs::load(const char *urls, const char *id) {
	debugFunction();

	// sanity check
	if (charstring::isNullOrEmpty(urls) || charstring::isNullOrEmpty(id)) {
		return NULL;
	}

	debugPrintf("urls: %s\n",urls);
	debugPrintf("id: %s\n",id);

	// separate the urls
	char		**url;
	uint64_t	urlcount;
	charstring::split(urls,",",true,&url,&urlcount);

	// for each url...
	for (uint64_t i=0; i<urlcount; i++) {

		const char	*u=url[i];

		debugPrintf("    url: %s\n",u);

		// parse out the protocol
	 	char	*protocol=NULL;
		const char	*colon=charstring::findFirst(u,':');
		if (colon) {
			protocol=charstring::duplicate(u,colon-u);
		}

		// try to load a config plugin for that protocol
	 	if (protocol) {
	 		loadConfig(protocol);
			delete[] protocol;
	 	}

		// fall back to the xml plugin if that plugin failed to load
		if (!cfg) {
			loadConfig("xmldom");
		}

		// load the configuration for the specified id
		if (cfg) {
			if (cfg->load(u,id)) {
				break;
			} else {
				delete cfg;
				cfg=NULL;
			}
		}
	}

	// clean up
	for (uint64_t i=0; i<urlcount; i++) {
		delete[] url[i];
	}
	delete[] url;

	// warn the user if the specified instance wasn't found
	if (!cfg) {
		stderror.printf("Couldn't find id %s.\n",id);
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
	modulename.append(SQLR);
	modulename.append("config_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		debugPrintf("failed to load config module: %s\n",module);
		char	*error=dl->getError();
		debugPrintf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the config itself
	stringbuffer	functionname;
	functionname.append("new_sqlrconfig_")->append(module);
	sqlrconfig *(*newConfig)()=
			(sqlrconfig *(*)())
				dl->getSymbol(functionname.getString());
	if (!newConfig) {
		debugPrintf("failed to create config: %s\n",module);
		char	*error=dl->getError();
		debugPrintf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		dl=NULL;
		return;
	}
	cfg=(*newConfig)();

#else
	#include "sqlrconfigassignments.cpp"
	{
		cfg=NULL;
	}
#endif
}
