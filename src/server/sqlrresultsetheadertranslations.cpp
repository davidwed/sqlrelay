// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrresultsetheadertranslationdeclarations.cpp"
	}
#endif

class sqlrresultsetheadertranslationplugin {
	public:
		sqlrresultsetheadertranslation	*rstr;
		dynamiclib			*dl;
		const char			*module;
};

class sqlrresultsetheadertranslationsprivate {
	friend class sqlrresultsetheadertranslations;
	private:
		sqlrservercontroller	*_cont;

		bool		_debug;

		singlylinkedlist
			< sqlrresultsetheadertranslationplugin * >	_tlist;

		const char	*_error;
};

sqlrresultsetheadertranslations::sqlrresultsetheadertranslations(
						sqlrservercontroller *cont) {
	debugFunction();
	pvt=new sqlrresultsetheadertranslationsprivate;
	pvt->_cont=cont;
	pvt->_debug=cont->getConfig()->getDebugResultSetHeaderTranslations();
	pvt->_error=NULL;
}

sqlrresultsetheadertranslations::~sqlrresultsetheadertranslations() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrresultsetheadertranslations::load(domnode *parameters) {
	debugFunction();

	unload();

	// run through the result set translation list
	for (domnode *resultsetheadertranslation=
					parameters->getFirstTagChild();
			!resultsetheadertranslation->isNullNode();
			resultsetheadertranslation=
				resultsetheadertranslation->
						getNextTagSibling()) {

		// load result set translation
		loadResultSetHeaderTranslation(resultsetheadertranslation);
	}

	return true;
}

void sqlrresultsetheadertranslations::unload() {
	debugFunction();
	for (singlylinkedlistnode
		< sqlrresultsetheadertranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		sqlrresultsetheadertranslationplugin	*sqlt=node->getValue();
		delete sqlt->rstr;
		delete sqlt->dl;
		delete sqlt;
	}
	pvt->_tlist.clear();
}

void sqlrresultsetheadertranslations::loadResultSetHeaderTranslation(
				domnode *resultsetheadertranslation) {
	debugFunction();

	// ignore non-resultsetheadertranslations
	if (charstring::compare(resultsetheadertranslation->getName(),
						"resultsetheadertranslation")) {
		return;
	}

	// get the result set translation name
	const char	*module=
			resultsetheadertranslation->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=resultsetheadertranslation->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	if (pvt->_debug) {
		stdoutput.printf("loading result set "
					"header translation: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the result set translation module
	stringbuffer	modulename;
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
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
					sqlrresultsetheadertranslations *,
					domnode *)=
		(sqlrresultsetheadertranslation *(*)
					(sqlrservercontroller *,
					sqlrresultsetheadertranslations *,
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
		(*newResultSetTranslation)
			(pvt->_cont,this,resultsetheadertranslation);

#else
	dynamiclib			*dl=NULL;
	sqlrresultsetheadertranslation	*rstr;
	#include "sqlrresultsetheadertranslationassignments.cpp"
	{
		rstr=NULL;
	}
#endif

	if (pvt->_debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrresultsetheadertranslationplugin	*sqlrrstp=
				new sqlrresultsetheadertranslationplugin;
	sqlrrstp->rstr=rstr;
	sqlrrstp->dl=dl;
	sqlrrstp->module=module;
	pvt->_tlist.append(sqlrrstp);
}

bool sqlrresultsetheadertranslations::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char ***columnnames,
					uint16_t **columnnamelengths,
					uint16_t **columntypes,
					const char ***columntypenames,
					uint16_t **columntypenamelengths,
					uint32_t **columnlengths,
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
					uint16_t **columntablelengths) {
	debugFunction();

	pvt->_error=NULL;

	for (singlylinkedlistnode
		< sqlrresultsetheadertranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		if (pvt->_debug) {
			stdoutput.printf("\nrunning translation:  %s...\n\n",
						node->getValue()->module);
		}

		if (!node->getValue()->rstr->run(sqlrcon,sqlrcur,
						colcount,
						columnnames,
						columnnamelengths,
						columntypes,
						columntypenames,
						columntypenamelengths,
						columnlengths,
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
						columntablelengths)) {
			pvt->_error=node->getValue()->rstr->getError();
			return false;
		}
	}
	return true;
}

const char *sqlrresultsetheadertranslations::getError() {
	return pvt->_error;
}

void sqlrresultsetheadertranslations::endTransaction(bool commit) {
	for (singlylinkedlistnode
		< sqlrresultsetheadertranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->rstr->endTransaction(commit);
	}
}

void sqlrresultsetheadertranslations::endSession() {
	for (singlylinkedlistnode
		< sqlrresultsetheadertranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->rstr->endSession();
	}
}
