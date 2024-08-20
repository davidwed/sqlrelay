// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrresultsetheadertranslationdeclarations.cpp"
	}
#endif

class sqlrresultsetheadertranslationsprivate {
	friend class sqlrresultsetheadertranslations;
	private:
		const char	*_error;
};

sqlrresultsetheadertranslations::sqlrresultsetheadertranslations(
						sqlrservercontroller *cont) :
						sqlrservermodules(cont) {
	pvt=new sqlrresultsetheadertranslationsprivate;
	setDebug(cont->getConfig()->getDebugResultSetHeaderTranslations());
	pvt->_error=NULL;
}

sqlrresultsetheadertranslations::~sqlrresultsetheadertranslations() {
	delete pvt;
}

void sqlrresultsetheadertranslations::loadModule(domnode *parameters) {

	// ignore non-resultsetheadertranslations
	if (charstring::compare(parameters->getName(),
					"resultsetheadertranslation")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading result set header translation: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the result set translation module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("resultsetheadertranslation_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load result set "
					"header translation module: %s\n",
					module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the result set translation itself
	stringbuffer	functionname;
	functionname.append("new_sqlrresultsetheadertranslation_")->
							append(module);
	sqlrresultsetheadertranslation *(*newResultSetTranslation)
					(sqlrservercontroller *,
					domnode *)=
		(sqlrresultsetheadertranslation *(*)
					(sqlrservercontroller *,
					domnode *))
				dl->getSymbol(functionname.getString());
	if (!newResultSetTranslation) {
		stdoutput.printf("failed to load result set "
					"header translation: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrresultsetheadertranslation	*rstr=
		(*newResultSetTranslation)(cont,parameters);

#else
	dynamiclib			*dl=NULL;
	sqlrresultsetheadertranslation	*rstr;
	#include "sqlrresultsetheadertranslationassignments.cpp"
	{
		rstr=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlrrstp=new sqlrmoduleplugin;
	sqlrrstp->m=rstr;
	sqlrrstp->dl=dl;
	sqlrrstp->module=module;
	blist.append(sqlrrstp);
}

bool sqlrresultsetheadertranslations::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char ***columnnames,
					uint16_t **columnnamesizes,
					uint16_t **columntypes,
					const char ***columntypenames,
					uint16_t **columntypenamesizes,
					uint32_t **columnsizes,
					uint32_t **columnprecisions,
					uint32_t **columnscales,
					uint16_t **columnisnullables,
					uint16_t **columnisprimarykeys,
					uint16_t **columnisuniques,
					uint16_t **columnispartofkeys,
					uint16_t **columnisunsigneds,
					uint16_t **columniszerofilleds,
					uint16_t **columnisbinarys,
					uint16_t **columnisautoincrements,
					const char ***columntables,
					uint16_t **columntablesizes) {

	pvt->_error=NULL;

	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {

		debugWrite("running result set header translation: %s...",
						node->getValue()->module);

		sqlrresultsetheadertranslation	*rstr=
				(sqlrresultsetheadertranslation *)
						node->getValue()->m;
		if (!rstr->run(sqlrcon,sqlrcur,
					colcount,
					columnnames,
					columnnamesizes,
					columntypes,
					columntypenames,
					columntypenamesizes,
					columnsizes,
					columnprecisions,
					columnscales,
					columnisnullables,
					columnisprimarykeys,
					columnisuniques,
					columnispartofkeys,
					columnisunsigneds,
					columniszerofilleds,
					columnisbinarys,
					columnisautoincrements,
					columntables,
					columntablesizes)) {
			pvt->_error=rstr->getError();
			return false;
		}
	}
	return true;
}

const char *sqlrresultsetheadertranslations::getError() {
	return pvt->_error;
}
