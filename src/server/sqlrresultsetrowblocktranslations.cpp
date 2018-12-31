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
		#include "sqlrresultsetrowblocktranslationdeclarations.cpp"
	}
#endif

class sqlrresultsetrowblocktranslationplugin {
	public:
		sqlrresultsetrowblocktranslation	*rstr;
		dynamiclib				*dl;
		const char				*module;
};

class sqlrresultsetrowblocktranslationsprivate {
	friend class sqlrresultsetrowblocktranslations;
	private:
		sqlrservercontroller	*_cont;

		bool			_debug;

		singlylinkedlist< sqlrresultsetrowblocktranslationplugin * >
									_tlist;

		uint64_t		_rowblocksize;
		uint64_t		_rowcount;

		const char		*_error;
};

sqlrresultsetrowblocktranslations::sqlrresultsetrowblocktranslations(
						sqlrservercontroller *cont) {
	debugFunction();
	pvt=new sqlrresultsetrowblocktranslationsprivate;
	pvt->_cont=cont;
	pvt->_debug=cont->getConfig()->getDebugResultSetRowBlockTranslations();
	pvt->_rowblocksize=0;
	pvt->_rowcount=0;
	pvt->_error=NULL;
}

sqlrresultsetrowblocktranslations::~sqlrresultsetrowblocktranslations() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrresultsetrowblocktranslations::load(domnode *parameters) {
	debugFunction();

	unload();

	pvt->_rowblocksize=charstring::toInteger(
				parameters->getAttributeValue("rowblocksize"));
	if (!pvt->_rowblocksize) {
		pvt->_rowblocksize=10;
	}
	

	// run through the result set translation list
	for (domnode *resultsetrowblocktranslation=
				parameters->getFirstTagChild();
			!resultsetrowblocktranslation->isNullNode();
			resultsetrowblocktranslation=
				resultsetrowblocktranslation->
						getNextTagSibling()) {

		// load result set translation
		loadResultSetRowBlockTranslation(resultsetrowblocktranslation);
	}

	return true;
}

void sqlrresultsetrowblocktranslations::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrresultsetrowblocktranslationplugin * >
					*node=pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		sqlrresultsetrowblocktranslationplugin	*sqlt=node->getValue();
		delete sqlt->rstr;
		delete sqlt->dl;
		delete sqlt;
	}
	pvt->_tlist.clear();
}

void sqlrresultsetrowblocktranslations::loadResultSetRowBlockTranslation(
					domnode *resultsetrowblocktranslation) {
	debugFunction();

	// ignore non-resultsetrowblocktranslations
	if (charstring::compare(resultsetrowblocktranslation->getName(),
					"resultsetrowblocktranslation")) {
		return;
	}

	// get the result set translation name
	const char	*module=
		resultsetrowblocktranslation->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=resultsetrowblocktranslation->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	if (pvt->_debug) {
		stdoutput.printf("loading result set "
				"row block translation: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the result set translation module
	stringbuffer	modulename;
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("resultsetrowblocktranslation_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load "
				"result set row block translation module: %s\n",
				module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the result set translation itself
	stringbuffer	functionname;
	functionname.append("new_sqlrresultsetrowblocktranslation_")->
							append(module);
	sqlrresultsetrowblocktranslation *(*newResultSetTranslation)
					(sqlrservercontroller *,
					sqlrresultsetrowblocktranslations *,
					domnode *)=
		(sqlrresultsetrowblocktranslation *(*)
					(sqlrservercontroller *,
					sqlrresultsetrowblocktranslations *,
					domnode *))
				dl->getSymbol(functionname.getString());
	if (!newResultSetTranslation) {
		stdoutput.printf("failed to load "
				"result set row block "
				"translation: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrresultsetrowblocktranslation	*rstr=
		(*newResultSetTranslation)
			(pvt->_cont,this,resultsetrowblocktranslation);

#else
	dynamiclib			*dl=NULL;
	sqlrresultsetrowblocktranslation	*rstr;
	#include "sqlrresultsetrowblocktranslationassignments.cpp"
	{
		rstr=NULL;
	}
#endif

	if (pvt->_debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrresultsetrowblocktranslationplugin	*sqlrrstp=
				new sqlrresultsetrowblocktranslationplugin;
	sqlrrstp->rstr=rstr;
	sqlrrstp->dl=dl;
	sqlrrstp->module=module;
	pvt->_tlist.append(sqlrrstp);
}

uint64_t sqlrresultsetrowblocktranslations::getRowBlockSize() {
	return pvt->_rowblocksize;
}

bool sqlrresultsetrowblocktranslations::setRow(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames,
						const char * const *fields,
						uint64_t *fieldlengths,
						bool *blobs,
						bool *nulls) {
	debugFunction();

	singlylinkedlistnode< sqlrresultsetrowblocktranslationplugin * >
						*node=pvt->_tlist.getFirst();
	if (!node) {
		return true;
	}

	if (pvt->_debug) {
		stdoutput.printf("\nrunning setRow():  %s...\n\n",
						node->getValue()->module);
	}

	pvt->_rowcount++;

	if (!node->getValue()->rstr->setRow(sqlrcon,sqlrcur,
						colcount,fieldnames,
						fields,fieldlengths,
						blobs,nulls)) {
		pvt->_rowcount=0;
		return false;
	}
	return true;
}

bool sqlrresultsetrowblocktranslations::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames) {
						
	debugFunction();

	pvt->_error=NULL;

	singlylinkedlistnode< sqlrresultsetrowblocktranslationplugin * >
						*node=pvt->_tlist.getFirst();
	if (!node) {
		return true;
	}
	if (pvt->_debug) {
		stdoutput.printf("\nrunning translation:  %s...\n\n",
						node->getValue()->module);
	}
	if (!node->getValue()->rstr->run(sqlrcon,sqlrcur,colcount,fieldnames)) {
		pvt->_rowcount=0;
		pvt->_error=node->getValue()->rstr->getError();
		return false;
	}

	for (singlylinkedlistnode< sqlrresultsetrowblocktranslationplugin * >
						*node=pvt->_tlist.getFirst();
						node; node=node->getNext()) {

		if (!node->getNext()) {
			break;
		}

		for (uint64_t i=0; i<pvt->_rowcount; i++) {

			if (pvt->_debug) {
				stdoutput.printf("\nrunning getRow():  "
						"%s...\n\n",
						node->getValue()->module);
			}

			const char	**oldfields;
			uint64_t	*oldfieldlengths;
			bool		*oldblobs;
			bool		*oldnulls;
			if (!node->getValue()->rstr->getRow(
							sqlrcon,
							sqlrcur,
							colcount,
							&oldfields,
							&oldfieldlengths,
							&oldblobs,
							&oldnulls)) {
				pvt->_rowcount=0;
				return false;
			}

			if (pvt->_debug) {
				stdoutput.printf("\nrunning setRow():  "
						"%s...\n\n",
						node->getValue()->module);
			}

			if (!node->getNext()->getValue()->rstr->setRow(
							sqlrcon,
							sqlrcur,
							colcount,
							fieldnames,
							oldfields,
							oldfieldlengths,
							oldblobs,
							oldnulls)) {
				pvt->_rowcount=0;
				return false;
			}
		}

		if (pvt->_debug) {
			stdoutput.printf("\nrunning translation:  %s...\n\n",
						node->getValue()->module);
		}

		if (!node->getNext()->getValue()->rstr->run(sqlrcon,
								sqlrcur,
								colcount,
								fieldnames)) {
			pvt->_rowcount=0;
			pvt->_error=node->getValue()->rstr->getError();
			return false;
		}
	}
	return true;
}

bool sqlrresultsetrowblocktranslations::getRow(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char ***fields,
						uint64_t **fieldlengths,
						bool **blobs,
						bool **nulls) {
	debugFunction();

	singlylinkedlistnode< sqlrresultsetrowblocktranslationplugin * >
						*node=pvt->_tlist.getLast();
	if (!node) {
		return true;
	}
	if (pvt->_debug) {
		stdoutput.printf("\nrunning getRow():  %s...\n\n",
						node->getValue()->module);
	}

	pvt->_rowcount--;

	if (!node->getValue()->rstr->getRow(sqlrcon,sqlrcur,
						colcount,
						fields,fieldlengths,
						blobs,nulls)) {
		pvt->_rowcount=0;
		return false;
	}
	return true;
}

const char *sqlrresultsetrowblocktranslations::getError() {
	return pvt->_error;
}

void sqlrresultsetrowblocktranslations::endTransaction(bool commit) {
	for (singlylinkedlistnode< sqlrresultsetrowblocktranslationplugin * >
					*node=pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->rstr->endTransaction(commit);
	}
}

void sqlrresultsetrowblocktranslations::endSession() {
	for (singlylinkedlistnode< sqlrresultsetrowblocktranslationplugin * >
					*node=pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->rstr->endSession();
	}
}
