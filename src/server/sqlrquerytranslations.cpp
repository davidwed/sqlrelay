// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES
#include <rudiments/debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrquerytranslationdeclarations.cpp"
	}
#endif

class sqlrquerytranslationplugin {
	public:
		sqlrquerytranslation	*tr;
		dynamiclib		*dl;
		const char		*module;
};

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
		sqlrservercontroller	*_cont;
		
		xmldom		*_tree;
		bool		_debug;

		const char	*_error;

		singlylinkedlist< sqlrquerytranslationplugin * >	_tlist;

		bool		_useoriginalonerror;

		dictionary< sqlrdatabaseobject *, char * >	_tablenamemap;
		dictionary< sqlrdatabaseobject *, char * >	_indexnamemap;
};

sqlrquerytranslations::sqlrquerytranslations(sqlrservercontroller *cont) {
	debugFunction();
	pvt=new sqlrquerytranslationsprivate;
	pvt->_cont=cont;
	pvt->_debug=cont->getConfig()->getDebugQueryTranslations();
	pvt->_error=NULL;
	pvt->_tree=NULL;
	pvt->_useoriginalonerror=true;
}

sqlrquerytranslations::~sqlrquerytranslations() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrquerytranslations::load(domnode *parameters) {
	debugFunction();

	unload();

	// default to useoriginal-on-error
	pvt->_useoriginalonerror=
		!charstring::compareIgnoringCase(
				parameters->getAttributeValue("onerror"),
				"original");

	// run through the translation list
	for (domnode *translation=parameters->getFirstTagChild();
				!translation->isNullNode();
				translation=translation->getNextTagSibling()) {

		// load translation
		loadTranslation(translation);
	}

	return true;
}

void sqlrquerytranslations::unload() {
	debugFunction();
	for (listnode< sqlrquerytranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		sqlrquerytranslationplugin	*sqlt=node->getValue();
		delete sqlt->tr;
		delete sqlt->dl;
		delete sqlt;
	}
	pvt->_tlist.clear();
}

void sqlrquerytranslations::loadTranslation(domnode *translation) {
	debugFunction();

	// ignore non-translations
	if (charstring::compare(translation->getName(),"querytranslation")) {
		return;
	}

	// get the translation name
	const char	*module=translation->getAttributeValue("module");
	if (!charstring::getLength(module)) {
		// try "file", that's what it used to be called
		module=translation->getAttributeValue("file");
		if (!charstring::getLength(module)) {
			return;
		}
	}

	if (pvt->_debug) {
		stdoutput.printf("loading translation module: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the translation module
	stringbuffer	modulename;
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("querytranslation_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {

		// try again with sqlrtranslation_...
		// (the old naming convention)
		modulename.clear();
		modulename.append(pvt->_cont->getPaths()->getLibExecDir());
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
						sqlrquerytranslations *,
						domnode *)=
		(sqlrquerytranslation *(*)(sqlrservercontroller *,
						sqlrquerytranslations *,
						domnode *))
				dl->getSymbol(functionname.getString());
	if (!newQueryTranslation) {

		// try again with new_sqlrtranslation_...
		// (the old naming convention)
		functionname.clear();
		functionname.append("new_sqlrtranslation_")->append(module);
		newQueryTranslation=
		(sqlrquerytranslation *(*)(sqlrservercontroller *,
						sqlrquerytranslations *,
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
	sqlrquerytranslation	*tr=(*newQueryTranslation)(pvt->_cont,this,
								translation);

#else
	dynamiclib	*dl=NULL;
	sqlrquerytranslation	*tr;
	#include "sqlrquerytranslationassignments.cpp"
	{
		tr=NULL;
	}
#endif

	if (pvt->_debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrquerytranslationplugin	*sqltp=new sqlrquerytranslationplugin;
	sqltp->tr=tr;
	sqltp->dl=dl;
	sqltp->module=module;
	pvt->_tlist.append(sqltp);
}

bool sqlrquerytranslations::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrparser *sqlrp,
					const char *query,
					uint32_t querylength,
					stringbuffer *translatedquery) {
	debugFunction();

	pvt->_error=NULL;

	// FIXME: I commented this out to allow empty queries to be sent
	// before getXXXList() calls.  Hopefully it doesn't break something
	// else...
	/*if (!querylength || !query) {
		pvt->_error="query was empty or null";
		if (pvt->_debug) {
			stdoutput.printf("\n%s\n\n",pvt->_error);
		}
		return false;
	}*/

	if (!translatedquery) {
		pvt->_error="buffer for translated query was null";
		if (pvt->_debug) {
			stdoutput.printf("\n%s\n\n",pvt->_error);
		}
		return false;
	}

	pvt->_tree=NULL;

	stringbuffer	tempquerystr1;
	stringbuffer	tempquerystr2;
	stringbuffer	*tempquerystr=&tempquerystr1;
	for (listnode< sqlrquerytranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {

		if (pvt->_debug) {
			stdoutput.printf("\nrunning translation:  %s...\n\n",
						node->getValue()->module);
		}

		sqlrquerytranslation	*tr=node->getValue()->tr;

		if (tr->usesTree()) {

			if (!sqlrp) {
				pvt->_error="query translation requires query "
						"tree but no parser available";
				if (pvt->_debug) {
					stdoutput.printf("\n%s\n\n",
							pvt->_error);
				}
				return false;
			}

			if (!pvt->_tree) {
				if (!sqlrp->parse(query)) {
					pvt->_error="parse-query failed";
					sqlrcon->cont->
						raiseParseFailureEvent(
								sqlrcur,query);
					return false;
				}
				pvt->_tree=sqlrp->getTree();
				if (pvt->_debug) {
					stdoutput.printf(
						"current query tree:\n");
					if (pvt->_tree) {
						pvt->_tree->getRootNode()->
							write(&stdoutput,true);
					}
					stdoutput.printf("\n");
				}
			}

			if (!tr->run(sqlrcon,sqlrcur,pvt->_tree)) {
				pvt->_error=tr->getError();
				if (pvt->_debug) {
					stdoutput.printf("\n%s\n\n",
							pvt->_error);
				}
				return false;
			}

		} else {
			tempquerystr->clear();

			if (pvt->_tree) {
				if (!sqlrp->write(tempquerystr)) {
					pvt->_error="write-query failed";
					if (pvt->_debug) {
						stdoutput.printf("\n%s\n\n",
								pvt->_error);
					}
					return false;
				}
				pvt->_tree=NULL;
				query=tempquerystr->getString();

				tempquerystr=(tempquerystr==&tempquerystr1)?
						&tempquerystr2:&tempquerystr1;
			}

			if (!tr->run(sqlrcon,sqlrcur,
					query,querylength,tempquerystr)) {
				pvt->_error=tr->getError();
				if (pvt->_debug) {
					stdoutput.printf("\n%s\n\n",
							pvt->_error);
				}
				return false;
			}

			query=tempquerystr->getString();
			querylength=tempquerystr->getSize();

			tempquerystr=(tempquerystr==&tempquerystr1)?
						&tempquerystr2:&tempquerystr1;
		}
	}

	if (pvt->_tree) {
		if (!sqlrp->write(translatedquery)) {
			pvt->_error="final write-query failed";
			if (pvt->_debug) {
				stdoutput.printf("current query tree:\n");
				if (pvt->_tree) {
					pvt->_tree->getRootNode()->
						write(&stdoutput,true);
				}
				stdoutput.printf("\n");
				stdoutput.printf("\n%s\n\n",pvt->_error);
			}
			return false;
		}
	} else {
		translatedquery->append(query,querylength);
		if (sqlrp->parse(translatedquery->getString())) {
			pvt->_tree=sqlrp->getTree();
		} else {
			sqlrcon->cont->raiseParseFailureEvent(sqlrcur,
						translatedquery->getString());
			// FIXME: shouldn't I return false if this happens?
		}
	}

	if (pvt->_debug) {
		stdoutput.printf("\nquery tree after translation:\n");
		if (pvt->_tree) {
			pvt->_tree->getRootNode()->write(&stdoutput,true);
		}
		stdoutput.printf("\n");
	}

	return true;
}

const char *sqlrquerytranslations::getError() {
	return pvt->_error;
}

sqlrdatabaseobject *sqlrquerytranslations::createDatabaseObject(
						const char *database,
						const char *schema,
						const char *object,
						const char *dependency) {

	// initialize copy pointers
	char	*databasecopy=NULL;
	char	*schemacopy=NULL;
	char	*objectcopy=NULL;
	char	*dependencycopy=NULL;

	// get memory pool
	memorypool	*pool=pvt->_cont->getPerSessionMemoryPool();

	// create buffers and copy data into them
	if (database) {
		databasecopy=(char *)pool->allocate(
				charstring::getLength(database)+1);
		charstring::copy(databasecopy,database);
	}
	if (schema) {
		schemacopy=(char *)pool->allocate(
				charstring::getLength(schema)+1);
		charstring::copy(schemacopy,schema);
	}
	if (object) {
		objectcopy=(char *)pool->allocate(
				charstring::getLength(object)+1);
		charstring::copy(objectcopy,object);
	}
	if (dependency) {
		dependencycopy=(char *)pool->allocate(
				charstring::getLength(dependency)+1);
		charstring::copy(dependencycopy,dependency);
	}

	// create the databaseobject
	sqlrdatabaseobject	*dbo=(sqlrdatabaseobject *)
				pool->allocate(sizeof(sqlrdatabaseobject));


	// populate it
	// (if placement new worked as expected on all platforms, we wouldn't
	// need to do this, we could pass them into the constructor or
	// use setters or something...)
	dbo->database=databasecopy;
	dbo->schema=schemacopy;
	dbo->object=objectcopy;
	dbo->dependency=dependencycopy;

	// return it
	return dbo;
}

void sqlrquerytranslations::setReplacementTableName(
					const char *database,
					const char *schema,
					const char *oldtable,
					const char *newtable) {
	setReplacementName(&pvt->_tablenamemap,
				createDatabaseObject(
					database,
					schema,
					oldtable,
					NULL),
				newtable);
}

void sqlrquerytranslations::setReplacementIndexName(
					const char *database,
					const char *schema,
					const char *oldindex,
					const char *newindex,
					const char *table) {
	setReplacementName(&pvt->_indexnamemap,
				createDatabaseObject(
					database,
					schema,
					oldindex,
					table),
				newindex);
}

void sqlrquerytranslations::setReplacementName(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				sqlrdatabaseobject *oldobject,
				const char *newobject) {
	dict->setValue(oldobject,(char *)newobject);
}

bool sqlrquerytranslations::getReplacementTableName(const char *database,
						const char *schema,
						const char *oldtable,
						const char **newtable) {
	return getReplacementName(&pvt->_tablenamemap,
					database,schema,
					oldtable,newtable);
}

bool sqlrquerytranslations::getReplacementIndexName(const char *database,
						const char *schema,
						const char *oldtable,
						const char **newtable) {
		return getReplacementName(&pvt->_indexnamemap,
						database,schema,
						oldtable,newtable);
}

bool sqlrquerytranslations::getReplacementName(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldobject,
				const char **newobject) {

	*newobject=NULL;
	for (listnode<sqlrdatabaseobject *> *node=dict->getKeys()->getFirst();
						node; node=node->getNext()) {

		sqlrdatabaseobject	*dbo=node->getValue();
		if (!charstring::compare(dbo->database,database) &&
			!charstring::compare(dbo->schema,schema) &&
			!charstring::compare(dbo->object,oldobject)) {
			*newobject=dict->getValue(dbo);
			return true;
		}
	}
	return false;
}

bool sqlrquerytranslations::removeReplacementTable(const char *database,
						const char *schema,
						const char *table) {

	// remove the table
	if (!removeReplacement(&pvt->_tablenamemap,database,schema,table)) {
		return false;
	}

	// remove any indices that depend on the table
	for (listnode<sqlrdatabaseobject *> *node=
			pvt->_indexnamemap.getKeys()->getFirst(); node;) {

		sqlrdatabaseobject	*dbo=node->getValue();

		// make sure to move on to the next node here rather than
		// after calling remove, otherwise it could cause a
		// reference-after-free condition
		node=node->getNext();

		if (!charstring::compare(dbo->database,database) &&
			!charstring::compare(dbo->schema,schema) &&
			!charstring::compare(dbo->dependency,table)) {

			pvt->_indexnamemap.remove(dbo);
		}
	}
	return true;
}

bool sqlrquerytranslations::removeReplacementIndex(const char *database,
						const char *schema,
						const char *index) {
	return removeReplacement(&pvt->_indexnamemap,database,schema,index);
}

bool sqlrquerytranslations::removeReplacement(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *object) {

	for (listnode<sqlrdatabaseobject *> *node=dict->getKeys()->getFirst();
						node; node=node->getNext()) {

		sqlrdatabaseobject	*dbo=node->getValue();
		const char		*replacementobject=dict->getValue(dbo);
		if (!charstring::compare(dbo->database,database) &&
			!charstring::compare(dbo->schema,schema) &&
			!charstring::compare(replacementobject,object)) {

			dict->remove(dbo);
			return true;
		}
	}
	return false;
}

bool sqlrquerytranslations::getUseOriginalOnError() {
	return pvt->_useoriginalonerror;
}

void sqlrquerytranslations::endTransaction(bool commit) {
	for (listnode< sqlrquerytranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->tr->endTransaction(commit);
	}
}

void sqlrquerytranslations::endSession() {
	pvt->_tablenamemap.clear();
	pvt->_indexnamemap.clear();
	for (listnode< sqlrquerytranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->tr->endSession();
	}
}
