// Copyright (c) 1999-2011  David Muse
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
		#include "sqlrresultsetrowtranslationdeclarations.cpp"
	}
#endif

class sqlrresultsetrowtranslationplugin {
	public:
		sqlrresultsetrowtranslation	*rstr;
		dynamiclib			*dl;
};

class sqlrresultsetrowtranslationsprivate {
	friend class sqlrresultsetrowtranslations;
	private:
		const char	*_libexecdir;
		bool		_debug;

		singlylinkedlist< sqlrresultsetrowtranslationplugin * >	_tlist;
};

sqlrresultsetrowtranslations::sqlrresultsetrowtranslations(sqlrpaths *sqlrpth,
								bool debug) {
	debugFunction();
	pvt=new sqlrresultsetrowtranslationsprivate;
	pvt->_libexecdir=sqlrpth->getLibExecDir();
	pvt->_debug=debug;
}

sqlrresultsetrowtranslations::~sqlrresultsetrowtranslations() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrresultsetrowtranslations::load(xmldomnode *parameters) {
	debugFunction();

	unload();

	// run through the result set translation list
	for (xmldomnode *resultsetrowtranslation=parameters->getFirstTagChild();
			!resultsetrowtranslation->isNullNode();
			resultsetrowtranslation=
				resultsetrowtranslation->getNextTagSibling()) {

		// load result set translation
		loadResultSetRowTranslation(resultsetrowtranslation);
	}

	return true;
}

void sqlrresultsetrowtranslations::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrresultsetrowtranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		sqlrresultsetrowtranslationplugin	*sqlt=node->getValue();
		delete sqlt->rstr;
		delete sqlt->dl;
		delete sqlt;
	}
	pvt->_tlist.clear();
}

void sqlrresultsetrowtranslations::loadResultSetRowTranslation(
				xmldomnode *resultsetrowtranslation) {
	debugFunction();

	// ignore non-resultsetrowtranslations
	if (charstring::compare(resultsetrowtranslation->getName(),
						"resultsetrowtranslation")) {
		return;
	}

	// get the result set translation name
	const char	*module=
			resultsetrowtranslation->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=resultsetrowtranslation->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	if (pvt->_debug) {
		stdoutput.printf("loading result set translation: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the result set translation module
	stringbuffer	modulename;
	modulename.append(pvt->_libexecdir);
	modulename.append(SQLR);
	modulename.append("resultsetrowtranslation_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load "
				"result set translation module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the result set translation itself
	stringbuffer	functionname;
	functionname.append("new_sqlrresultsetrowtranslation_")->append(module);
	sqlrresultsetrowtranslation *(*newResultSetTranslation)
		(sqlrresultsetrowtranslations *, xmldomnode *, bool)=
		(sqlrresultsetrowtranslation *(*)
		(sqlrresultsetrowtranslations *, xmldomnode *, bool))
				dl->getSymbol(functionname.getString());
	if (!newResultSetTranslation) {
		stdoutput.printf("failed to create "
				"result set translation: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrresultsetrowtranslation	*rstr=
		(*newResultSetTranslation)(
			this,resultsetrowtranslation,pvt->_debug);

#else
	dynamiclib			*dl=NULL;
	sqlrresultsetrowtranslation	*rstr;
	#include "sqlrresultsetrowtranslationassignments.cpp"
	{
		rstr=NULL;
	}
#endif

	if (pvt->_debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrresultsetrowtranslationplugin	*sqlrrstp=
				new sqlrresultsetrowtranslationplugin;
	sqlrrstp->rstr=rstr;
	sqlrrstp->dl=dl;
	pvt->_tlist.append(sqlrrstp);
}

bool sqlrresultsetrowtranslations::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames,
						const char ***fields,
						uint64_t **fieldlengths) {
	debugFunction();

	for (singlylinkedlistnode< sqlrresultsetrowtranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		if (pvt->_debug) {
			stdoutput.printf(
				"\nrunning result set row translation...\n\n");
		}

		if (!node->getValue()->rstr->run(sqlrcon,sqlrcur,
						colcount,fieldnames,
						fields,fieldlengths)) {
			return false;
		}
	}
	return true;
}
