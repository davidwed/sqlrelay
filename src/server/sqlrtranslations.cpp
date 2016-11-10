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
		const char	*_libexecdir;
		xmldom		*_tree;
		bool		_debug;

		singlylinkedlist< sqlrtranslationplugin * >	_tlist;

		memorypool	*_tablenamepool;
		memorypool	*_indexnamepool;

		dictionary< sqlrdatabaseobject *, char * >	_tablenamemap;
		dictionary< sqlrdatabaseobject *, char * >	_indexnamemap;
};

sqlrtranslations::sqlrtranslations(sqlrpaths *sqlrpth, bool debug) {
	debugFunction();
	pvt=new sqlrtranslationsprivate;
	pvt->_tree=NULL;
	pvt->_debug=debug;
	pvt->_tablenamepool=new memorypool(0,128,100);
	pvt->_indexnamepool=new memorypool(0,128,100);
	pvt->_libexecdir=sqlrpth->getLibExecDir();
}

sqlrtranslations::~sqlrtranslations() {
	debugFunction();
	unload();
	delete pvt->_tablenamepool;
	delete pvt->_indexnamepool;
	delete pvt;
}

bool sqlrtranslations::load(xmldomnode *parameters) {
	debugFunction();

	unload();

	// run through the translation list
	for (xmldomnode *translation=parameters->getFirstTagChild();
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

void sqlrtranslations::loadTranslation(xmldomnode *translation) {
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
	modulename.append(pvt->_libexecdir);
	modulename.append(SQLR);
	modulename.append("translation_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load "
				"translation module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the translation itself
	stringbuffer	functionname;
	functionname.append("new_sqlrtranslation_")->append(module);
	sqlrtranslation *(*newTranslation)
		(sqlrtranslations *, xmldomnode *, bool)=
		(sqlrtranslation *(*)(sqlrtranslations *, xmldomnode *, bool))
				dl->getSymbol(functionname.getString());
	if (!newTranslation) {
		stdoutput.printf("failed to create translation: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrtranslation	*tr=(*newTranslation)(this,translation,pvt->_debug);

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

	stringbuffer	tempquerystr;
	for (singlylinkedlistnode< sqlrtranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {

		if (pvt->_debug) {
			stdoutput.printf("\nrunning translation...\n\n");
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
					return false;
				}
				pvt->_tree=sqlrp->getTree();
				if (pvt->_debug) {
					stdoutput.printf(
						"current query tree:\n");
					if (pvt->_tree) {
						pvt->_tree->getRootNode()->
							print(&stdoutput);
					}
					stdoutput.printf("\n");
				}
			}

			if (!tr->run(sqlrcon,sqlrcur,pvt->_tree)) {
				return false;
			}

		} else {

			bool	freequery=false;
			if (pvt->_tree) {
				if (!sqlrp->write(&tempquerystr)) {
					return false;
				}
				pvt->_tree=NULL;
				query=tempquerystr.detachString();
				freequery=true;
			}

			bool	success=tr->run(sqlrcon,sqlrcur,
						query,&tempquerystr);
			if (freequery) {
				delete[] query;
			}

			if (!success) {
				return false;
			}

			query=tempquerystr.getString();
		}
	}

	if (pvt->_tree) {
		if (!sqlrp->write(translatedquery)) {
			return false;
		}
	} else {
		translatedquery->append(query);
		if (sqlrp->parse(translatedquery->getString())) {
			pvt->_tree=sqlrp->getTree();
		}
	}

	if (pvt->_debug) {
		stdoutput.printf("\nquery tree after translation:\n");
		if (pvt->_tree) {
			pvt->_tree->getRootNode()->print(&stdoutput);
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
				createDatabaseObject(pvt->_tablenamepool,
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
				createDatabaseObject(pvt->_indexnamepool,
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

memorypool *sqlrtranslations::getTableNamePool() {
	return pvt->_tablenamepool;
}

memorypool *sqlrtranslations::getIndexNamePool() {
	return pvt->_indexnamepool;
}

void sqlrtranslations::endSession() {
	pvt->_tablenamepool->deallocate();
	pvt->_indexnamepool->deallocate();
	pvt->_tablenamemap.clear();
	pvt->_indexnamemap.clear();
}
