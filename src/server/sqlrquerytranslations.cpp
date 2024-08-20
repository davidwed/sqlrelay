// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrquerytranslationdeclarations.cpp"
	}
#endif

class sqlrdatabaseobject {
	public:
		const char	*database;
		const char	*schema;
		const char	*object;
		const char	*dependency;
};

class sqlrquerytranslationsprivate {
	friend class sqlrquerytranslations;
	private:
		xmldom		*_tree;
		const char	*_error;
		bool		_useoriginalonerror;
};

sqlrquerytranslations::sqlrquerytranslations(sqlrservercontroller *cont) :
						sqlrservermodules(cont) {
	pvt=new sqlrquerytranslationsprivate;
	setDebug(cont->getConfig()->getDebugQueryTranslations());
	pvt->_error=NULL;
	pvt->_tree=NULL;
	pvt->_useoriginalonerror=true;
}

sqlrquerytranslations::~sqlrquerytranslations() {
	delete pvt;
}

bool sqlrquerytranslations::load(domnode *parameters) {

	// default to useoriginal-on-error
	pvt->_useoriginalonerror=
		!charstring::compareIgnoringCase(
				parameters->getAttributeValue("onerror"),
				"original");

	return sqlrservermodules::load(parameters);
}

void sqlrquerytranslations::loadModule(domnode *parameters) {

	// ignore non-translations
	if (charstring::compare(parameters->getName(),"querytranslation")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading query translation module: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the translation module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("querytranslation_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {

		// try again with sqlrtranslation_...
		// (the old naming convention)
		modulename.clear();
		modulename.append(cont->getPaths()->getLibExecDir());
		modulename.append(SQLR);
		modulename.append("translation_");
		modulename.append(module)->append(".");
		modulename.append(SQLRELAY_MODULESUFFIX);
		if (!dl->open(modulename.getString(),true,true)) {
			stdoutput.printf("failed to load "
				"query translation module: %s\n",module);
			char	*error=dl->getError();
			stdoutput.printf("%s\n",(error)?error:"");
			delete[] error;
			delete dl;
			return;
		}
	}

	// load the translation itself
	stringbuffer	functionname;
	functionname.append("new_sqlrquerytranslation_")->append(module);
	sqlrquerytranslation *(*newQueryTranslation)(sqlrservercontroller *,
						domnode *)=
		(sqlrquerytranslation *(*)(sqlrservercontroller *,
						domnode *))
				dl->getSymbol(functionname.getString());
	if (!newQueryTranslation) {

		// try again with new_sqlrtranslation_...
		// (the old naming convention)
		functionname.clear();
		functionname.append("new_sqlrtranslation_")->append(module);
		newQueryTranslation=
		(sqlrquerytranslation *(*)(sqlrservercontroller *,
						domnode *))
				dl->getSymbol(functionname.getString());
		if (!newQueryTranslation) {
			stdoutput.printf("failed to load query "
						"translation: %s\n",module);
			char	*error=dl->getError();
			stdoutput.printf("%s\n",(error)?error:"");
			delete[] error;
			dl->close();
			delete dl;
			return;
		}
	}
	sqlrquerytranslation	*tr=(*newQueryTranslation)(cont,parameters);

#else
	dynamiclib	*dl=NULL;
	sqlrquerytranslation	*tr;
	#include "sqlrquerytranslationassignments.cpp"
	{
		tr=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlrmpp=new sqlrmoduleplugin;
	sqlrmpp->m=tr;
	sqlrmpp->dl=dl;
	sqlrmpp->module=module;
	blist.append(sqlrmpp);
}

bool sqlrquerytranslations::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrparser *sqlrp,
					const char *query,
					uint32_t querysize,
					stringbuffer *translatedquery) {

	pvt->_error=NULL;

	// FIXME: I commented this out to allow empty queries to be sent
	// before getXXXList() calls.  Hopefully it doesn't break something
	// else...
	/*if (!querysize || !query) {
		pvt->_error="query was empty or null";
		debugWrite(pvt->_error);
		return false;
	}*/

	if (!translatedquery) {
		pvt->_error="buffer for translated query was null";
		debugWrite(pvt->_error);
		return false;
	}

	pvt->_tree=NULL;

	stringbuffer	tempquerystr1;
	stringbuffer	tempquerystr2;
	stringbuffer	*tempquerystr=&tempquerystr1;
	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {

		debugWrite("running query translation: %s...",
						node->getValue()->module);

		sqlrquerytranslation	*tr=
			(sqlrquerytranslation *)node->getValue()->m;

		if (tr->requiresTree()) {

			if (!sqlrp) {
				pvt->_error="query translation requires query "
						"tree but no parser available";
				debugWrite(pvt->_error);
				return false;
			}

			if (!pvt->_tree) {
				if (!sqlrp->parse(query)) {
					pvt->_error="query translation "
							"requires query tree "
							"but query failed to "
							"parse";
					sqlrcon->cont->
						raiseParseFailureEvent(
								sqlrcur,query);
					return false;
				}
				pvt->_tree=sqlrp->getQueryTree();
				if (getDebug()) {
					debugWrite("current query tree:");
					stringbuffer	b;
					pvt->_tree->getRootNode()->
							write(&b,true);
					debugWrite(b.getString());
				}
			}

			if (!tr->run(sqlrcon,sqlrcur,pvt->_tree)) {
				pvt->_error=tr->getError();
				debugWrite(pvt->_error);
				return false;
			}

		} else {
			tempquerystr->clear();

			if (pvt->_tree) {
				if (!sqlrp->write(tempquerystr)) {
					pvt->_error="write-query failed";
					debugWrite(pvt->_error);
					return false;
				}
				pvt->_tree=NULL;
				query=tempquerystr->getString();

				tempquerystr=(tempquerystr==&tempquerystr1)?
						&tempquerystr2:&tempquerystr1;
			}

			if (!tr->run(sqlrcon,sqlrcur,
					query,querysize,tempquerystr)) {
				pvt->_error=tr->getError();
				debugWrite(pvt->_error);
				return false;
			}

			query=tempquerystr->getString();
			querysize=tempquerystr->getSize();

			tempquerystr=(tempquerystr==&tempquerystr1)?
						&tempquerystr2:&tempquerystr1;
		}
	}

	if (pvt->_tree) {
		if (!sqlrp->write(translatedquery)) {
			pvt->_error="final write-query failed";
			if (getDebug()) {
				debugWrite("current query tree:");
				stringbuffer	b;
				pvt->_tree->getRootNode()->
						write(&b,true);
				debugWrite(b.getString());
				debugWrite(pvt->_error);
			}
			return false;
		}
	} else {
		translatedquery->append(query,querysize);
		if (sqlrp->parse(translatedquery->getString())) {
			pvt->_tree=sqlrp->getQueryTree();
		} else {
			sqlrcon->cont->raiseParseFailureEvent(sqlrcur,
						translatedquery->getString());
			// FIXME: shouldn't I return false if this happens?
		}
	}

	if (getDebug()) {
		debugWrite("query tree after translation:");
		if (pvt->_tree) {
			stringbuffer	b;
			pvt->_tree->getRootNode()->write(&b,true);
			debugWrite(b.getString());
		}
	}

	return true;
}

const char *sqlrquerytranslations::getError() {
	return pvt->_error;
}

bool sqlrquerytranslations::getUseOriginalOnError() {
	return pvt->_useoriginalonerror;
}
