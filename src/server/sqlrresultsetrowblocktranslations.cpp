// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrresultsetrowblocktranslationdeclarations.cpp"
	}
#endif

class sqlrresultsetrowblocktranslationsprivate {
	friend class sqlrresultsetrowblocktranslations;
	private:
		uint64_t	_rowblockcount;
		uint64_t	_rowcount;
		const char	*_error;
};

sqlrresultsetrowblocktranslations::sqlrresultsetrowblocktranslations(
						sqlrservercontroller *cont) :
						sqlrservermodules(cont) {
	pvt=new sqlrresultsetrowblocktranslationsprivate;
	setDebug(cont->getConfig()->getDebugResultSetRowBlockTranslations());
	pvt->_rowblockcount=0;
	pvt->_rowcount=0;
	pvt->_error=NULL;
}

sqlrresultsetrowblocktranslations::~sqlrresultsetrowblocktranslations() {
	delete pvt;
}

bool sqlrresultsetrowblocktranslations::load(domnode *parameters) {

	// get rowblockcount parameter
	pvt->_rowblockcount=charstring::convertToInteger(
			parameters->getAttributeValue("rowblockcount"));

	// try "rowblocksize", that's what it used to be called
	if (pvt->_rowblockcount) {
		pvt->_rowblockcount=charstring::convertToInteger(
			parameters->getAttributeValue("rowblocksize"));
	}

	// default to 10
	if (!pvt->_rowblockcount) {
		pvt->_rowblockcount=10;
	}

	return sqlrservermodules::load(parameters);
}

void sqlrresultsetrowblocktranslations::loadModule(domnode *parameters) {

	// ignore non-resultsetrowblocktranslations
	if (charstring::compare(parameters->getName(),
					"resultsetrowblocktranslation")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading result set row block translation: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the result set translation module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
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
					domnode *)=
		(sqlrresultsetrowblocktranslation *(*)
					(sqlrservercontroller *,
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
		(*newResultSetTranslation)(cont,parameters);

#else
	dynamiclib			*dl=NULL;
	sqlrresultsetrowblocktranslation	*rstr;
	#include "sqlrresultsetrowblocktranslationassignments.cpp"
	{
		rstr=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlmp=new sqlrmoduleplugin;
	sqlmp->m=rstr;
	sqlmp->dl=dl;
	sqlmp->module=module;
	blist.append(sqlmp);
}

uint64_t sqlrresultsetrowblocktranslations::getRowBlockCount() {
	return pvt->_rowblockcount;
}

bool sqlrresultsetrowblocktranslations::setRow(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames,
						const char * const *fields,
						uint64_t *fieldsizes,
						bool *lobs,
						bool *nulls) {

	listnode< sqlrmoduleplugin * > *node=blist.getFirst();
	if (!node) {
		return true;
	}

	debugWrite("running setRow(): %s...",node->getValue()->module);

	pvt->_rowcount++;

	sqlrresultsetrowblocktranslation	*rstr=
		(sqlrresultsetrowblocktranslation *)node->getValue()->m;
	if (!rstr->setRow(sqlrcon,sqlrcur,
				colcount,fieldnames,
				fields,fieldsizes,
				lobs,nulls)) {
		pvt->_rowcount=0;
		return false;
	}
	return true;
}

bool sqlrresultsetrowblocktranslations::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames) {
						

	pvt->_error=NULL;

	listnode< sqlrmoduleplugin * > *node=blist.getFirst();
	if (!node) {
		return true;
	}

	sqlrresultsetrowblocktranslation	*rstr=
		(sqlrresultsetrowblocktranslation *)node->getValue()->m;
	if (!rstr->run(sqlrcon,sqlrcur,colcount,fieldnames)) {
		pvt->_rowcount=0;
		pvt->_error=rstr->getError();
		return false;
	}

	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {

		if (!node->getNext()) {
			break;
		}

		for (uint64_t i=0; i<pvt->_rowcount; i++) {

			const char	**oldfields;
			uint64_t	*oldfieldsizes;
			bool		*oldlobs;
			bool		*oldnulls;
			rstr=(sqlrresultsetrowblocktranslation *)
						node->getValue()->m;
			if (!rstr->getRow(sqlrcon,
						sqlrcur,
						colcount,
						&oldfields,
						&oldfieldsizes,
						&oldlobs,
						&oldnulls)) {
				pvt->_rowcount=0;
				return false;
			}

			if (!rstr->setRow(sqlrcon,
						sqlrcur,
						colcount,
						fieldnames,
						oldfields,
						oldfieldsizes,
						oldlobs,
						oldnulls)) {
				pvt->_rowcount=0;
				return false;
			}
		}

		debugWrite("running result set row block translation: %s...",
						node->getValue()->module);

		if (!rstr->run(sqlrcon,sqlrcur,colcount,fieldnames)) {
			pvt->_rowcount=0;
			pvt->_error=rstr->getError();
			return false;
		}
	}
	return true;
}

bool sqlrresultsetrowblocktranslations::getRow(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char ***fields,
						uint64_t **fieldsizes,
						bool **lobs,
						bool **nulls) {

	listnode< sqlrmoduleplugin * > *node=blist.getLast();
	if (!node) {
		return true;
	}

	debugWrite("running getRow(): %s...",node->getValue()->module);

	pvt->_rowcount--;

	sqlrresultsetrowblocktranslation	*rstr=
		(sqlrresultsetrowblocktranslation *)node->getValue()->m;
	if (!rstr->getRow(sqlrcon,sqlrcur,
				colcount,
				fields,fieldsizes,
				lobs,nulls)) {
		pvt->_rowcount=0;
		return false;
	}
	return true;
}

const char *sqlrresultsetrowblocktranslations::getError() {
	return pvt->_error;
}
