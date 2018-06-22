// Copyright (c) 1999-2011  David Muse
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
		#include "sqlrtranslationdeclarations.cpp"
	}
#endif

class sqlrtranslationplugin {
	public:
		sqlrtranslation	*tr;
		dynamiclib	*dl;
		const char	*module;
};

class sqlrdatabaseobject {
	public:
		const char	*database;
		const char	*schema;
		const char	*object;
		const char	*dependency;
};

class sqlrtranslationsprivate {
	friend class sqlrtranslations;
	private:
		sqlrservercontroller	*_cont;
		
		xmldom		*_tree;
		bool		_debug;

		singlylinkedlist< sqlrtranslationplugin * >	_tlist;

		memorypool	*_memorypool;

		dictionary< sqlrdatabaseobject *, char * >	_tablenamemap;
		dictionary< sqlrdatabaseobject *, char * >	_indexnamemap;
};

sqlrtranslations::sqlrtranslations(sqlrservercontroller *cont) {
	debugFunction();
	pvt=new sqlrtranslationsprivate;
	pvt->_cont=cont;
	pvt->_debug=cont->getConfig()->getDebugTranslations();
	pvt->_tree=NULL;
	pvt->_memorypool=new memorypool(0,128,100);
}

sqlrtranslations::~sqlrtranslations() {
	debugFunction();
	unload();
	delete pvt->_memorypool;
	delete pvt;
}

bool sqlrtranslations::load(domnode *parameters) {
	debugFunction();

	unload();

	// run through the translation list
	for (domnode *translation=parameters->getFirstTagChild();
				!translation->isNullNode();
				translation=translation->getNextTagSibling()) {

		// load translation
		loadTranslation(translation);
	}

	return true;
}

void sqlrtranslations::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrtranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		sqlrtranslationplugin	*sqlt=node->getValue();
		delete sqlt->tr;
		delete sqlt->dl;
		delete sqlt;
	}
	pvt->_tlist.clear();
}

void sqlrtranslations::loadTranslation(domnode *translation) {
	debugFunction();

	// ignore non-translations
	if (charstring::compare(translation->getName(),"translation")) {
		return;
	}

	// get the translation name
	const char	*module=translation->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=translation->getAttributeValue("file");
		if (!charstring::length(module)) {
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
	modulename.append("translation_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load "
				"translation module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the translation itself
	stringbuffer	functionname;
	functionname.append("new_sqlrtranslation_")->append(module);
	sqlrtranslation *(*newTranslation)(sqlrservercontroller *,
						sqlrtranslations *,
						domnode *)=
		(sqlrtranslation *(*)(sqlrservercontroller *,
						sqlrtranslations *,
						domnode *))
				dl->getSymbol(functionname.getString());
	if (!newTranslation) {
		stdoutput.printf("failed to load translation: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrtranslation	*tr=(*newTranslation)(pvt->_cont,this,translation);

#else
	dynamiclib	*dl=NULL;
	sqlrtranslation	*tr;
	#include "sqlrtranslationassignments.cpp"
	{
		tr=NULL;
	}
#endif

	if (pvt->_debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrtranslationplugin	*sqltp=new sqlrtranslationplugin;
	sqltp->tr=tr;
	sqltp->dl=dl;
	sqltp->module=module;
	pvt->_tlist.append(sqltp);
}

bool sqlrtranslations::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrparser *sqlrp,
					const char *query,
					stringbuffer *translatedquery) {
	debugFunction();

	if (!query || !translatedquery) {
		return false;
	}

	pvt->_tree=NULL;

	stringbuffer	tempquerystr1;
	stringbuffer	tempquerystr2;
	stringbuffer	*tempquerystr=&tempquerystr1;
	for (singlylinkedlistnode< sqlrtranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {

		if (pvt->_debug) {
			stdoutput.printf("\nrunning translation:  %s...\n\n",
						node->getValue()->module);
		}

		sqlrtranslation	*tr=node->getValue()->tr;

		if (tr->usesTree()) {

			if (!sqlrp) {
				if (pvt->_debug) {
					stdoutput.printf("\ntranslation "
							"requires query tree "
							"but no parser "
							"available...\n\n");
				}
				return false;
			}

			if (!pvt->_tree) {
				if (!sqlrp->parse(query)) {
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
				return false;
			}

		} else {
			tempquerystr->clear();

			if (pvt->_tree) {
				if (!sqlrp->write(tempquerystr)) {
					return false;
				}
				pvt->_tree=NULL;
				query=tempquerystr->getString();

				tempquerystr=(tempquerystr==&tempquerystr1)?
						&tempquerystr2:&tempquerystr1;
			}

			bool	success=tr->run(sqlrcon,sqlrcur,
						query,tempquerystr);
			if (!success) {
				return false;
			}

			query=tempquerystr->getString();

			tempquerystr=(tempquerystr==&tempquerystr1)?
						&tempquerystr2:&tempquerystr1;
		}
	}

	if (pvt->_tree) {
		if (!sqlrp->write(translatedquery)) {
			if (pvt->_debug) {
				stdoutput.printf("current query tree:\n");
				if (pvt->_tree) {
					pvt->_tree->getRootNode()->
						write(&stdoutput,true);
				}
				stdoutput.printf("\n");
				stdoutput.printf("\nfinal write failed\n\n");
			}
			return false;
		}
	} else {
		translatedquery->append(query);
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

sqlrdatabaseobject *sqlrtranslations::createDatabaseObject(memorypool *pool,
						const char *database,
						const char *schema,
						const char *object,
						const char *dependency) {

	// initialize copy pointers
	char	*databasecopy=NULL;
	char	*schemacopy=NULL;
	char	*objectcopy=NULL;
	char	*dependencycopy=NULL;

	// create buffers and copy data into them
	if (database) {
		databasecopy=(char *)pool->allocate(
				charstring::length(database)+1);
		charstring::copy(databasecopy,database);
	}
	if (schema) {
		schemacopy=(char *)pool->allocate(
				charstring::length(schema)+1);
		charstring::copy(schemacopy,schema);
	}
	if (object) {
		objectcopy=(char *)pool->allocate(
				charstring::length(object)+1);
		charstring::copy(objectcopy,object);
	}
	if (dependency) {
		dependencycopy=(char *)pool->allocate(
				charstring::length(dependency)+1);
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

void sqlrtranslations::setReplacementTableName(
					const char *database,
					const char *schema,
					const char *oldtable,
					const char *newtable) {
	setReplacementName(&pvt->_tablenamemap,
				createDatabaseObject(pvt->_memorypool,
							database,
							schema,
							oldtable,
							NULL),
				newtable);
}

void sqlrtranslations::setReplacementIndexName(
					const char *database,
					const char *schema,
					const char *oldindex,
					const char *newindex,
					const char *table) {
	setReplacementName(&pvt->_indexnamemap,
				createDatabaseObject(pvt->_memorypool,
							database,
							schema,
							oldindex,
							table),
				newindex);
}

void sqlrtranslations::setReplacementName(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				sqlrdatabaseobject *oldobject,
				const char *newobject) {
	dict->setValue(oldobject,(char *)newobject);
}

bool sqlrtranslations::getReplacementTableName(const char *database,
						const char *schema,
						const char *oldtable,
						const char **newtable) {
	return getReplacementName(&pvt->_tablenamemap,
					database,schema,
					oldtable,newtable);
}

bool sqlrtranslations::getReplacementIndexName(const char *database,
						const char *schema,
						const char *oldtable,
						const char **newtable) {
		return getReplacementName(&pvt->_indexnamemap,
						database,schema,
						oldtable,newtable);
}

bool sqlrtranslations::getReplacementName(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldobject,
				const char **newobject) {

	*newobject=NULL;
	for (linkedlistnode< dictionarynode< sqlrdatabaseobject *, char * > *>
					*node=dict->getList()->getFirst();
						node; node=node->getNext()) {

		sqlrdatabaseobject	*dbo=node->getValue()->getKey();
		if (!charstring::compare(dbo->database,database) &&
			!charstring::compare(dbo->schema,schema) &&
			!charstring::compare(dbo->object,oldobject)) {
			*newobject=node->getValue()->getValue();
			return true;
		}
	}
	return false;
}

bool sqlrtranslations::removeReplacementTable(const char *database,
						const char *schema,
						const char *table) {

	// remove the table
	if (!removeReplacement(&pvt->_tablenamemap,database,schema,table)) {
		return false;
	}

	// remove any indices that depend on the table
	for (linkedlistnode< dictionarynode< sqlrdatabaseobject *, char * > *>
			*node=pvt->_indexnamemap.getList()->getFirst(); node;) {

		sqlrdatabaseobject	*dbo=node->getValue()->getKey();

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

bool sqlrtranslations::removeReplacementIndex(const char *database,
						const char *schema,
						const char *index) {
	return removeReplacement(&pvt->_indexnamemap,database,schema,index);
}

bool sqlrtranslations::removeReplacement(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *object) {

	for (linkedlistnode< dictionarynode< sqlrdatabaseobject *, char * > *>
				*node=dict->getList()->getFirst();
					node; node=node->getNext()) {

		sqlrdatabaseobject	*dbo=node->getValue()->getKey();
		const char	*replacementobject=node->getValue()->getValue();
		if (!charstring::compare(dbo->database,database) &&
			!charstring::compare(dbo->schema,schema) &&
			!charstring::compare(replacementobject,object)) {

			dict->remove(dbo);
			return true;
		}
	}
	return false;
}

memorypool *sqlrtranslations::getMemoryPool() {
	return pvt->_memorypool;
}

void sqlrtranslations::endSession() {
	pvt->_memorypool->deallocate();
	pvt->_tablenamemap.clear();
	pvt->_indexnamemap.clear();
}
