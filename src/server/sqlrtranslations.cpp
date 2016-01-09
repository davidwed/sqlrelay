// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <debugprint.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrtranslationdeclarations.cpp"
	}
#endif

sqlrtranslations::sqlrtranslations(sqlrpaths *sqlrpth, bool debug) {
	debugFunction();
	xmld=NULL;
	tree=NULL;
	this->debug=debug;
	temptablepool=new memorypool(0,128,100);
	tempindexpool=new memorypool(0,128,100);
	libexecdir=sqlrpth->getLibExecDir();
}

sqlrtranslations::~sqlrtranslations() {
	debugFunction();
	unloadTranslations();
	delete xmld;
	delete temptablepool;
	delete tempindexpool;
}

bool sqlrtranslations::loadTranslations(const char *translations) {
	debugFunction();

	unloadTranslations();

	// create the parser
	delete xmld;
	xmld=new xmldom();

	// parse the translations
	if (!xmld->parseString(translations)) {
		return false;
	}

	// get the translations tag
	xmldomnode	*translationsnode=
			xmld->getRootNode()->getFirstTagChild("translations");
	if (translationsnode->isNullNode()) {
		return false;
	}

	// run through the translation list
	for (xmldomnode *translation=translationsnode->getFirstTagChild();
				!translation->isNullNode();
				translation=translation->getNextTagSibling()) {

		// load translation
		loadTranslation(translation);
	}

	return true;
}

void sqlrtranslations::unloadTranslations() {
	debugFunction();
	for (singlylinkedlistnode< sqlrtranslationplugin * > *node=
						tlist.getFirst();
						node; node=node->getNext()) {
		sqlrtranslationplugin	*sqlt=node->getValue();
		delete sqlt->tr;
		delete sqlt->dl;
		delete sqlt;
	}
	tlist.clear();
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

	if (debug) {
		stdoutput.printf("loading translation module: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the translation module
	stringbuffer	modulename;
	modulename.append(libexecdir);
	modulename.append("sqlrtranslation_");
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
	sqlrtranslation	*tr=(*newTranslation)(this,translation,debug);

#else
	dynamiclib	*dl=NULL;
	sqlrtranslation	*tr;
	#include "sqlrtranslationassignments.cpp"
	{
		tr=NULL;
	}
#endif

	if (debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrtranslationplugin	*sqltp=new sqlrtranslationplugin;
	sqltp->tr=tr;
	sqltp->dl=dl;
	tlist.append(sqltp);
}

bool sqlrtranslations::runTranslations(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrparser *sqlrp,
					const char *query,
					stringbuffer *translatedquery) {
	debugFunction();

	if (!query || !translatedquery) {
		return false;
	}

	tree=NULL;

	stringbuffer	tempquerystr;
	for (singlylinkedlistnode< sqlrtranslationplugin * > *node=
						tlist.getFirst();
						node; node=node->getNext()) {

		if (debug) {
			stdoutput.printf("\nrunning translation...\n\n");
		}

		sqlrtranslation	*tr=node->getValue()->tr;

		if (tr->usesTree()) {

			if (!tree && sqlrp) {
				if (!sqlrp->parse(query)) {
					return false;
				}
				tree=sqlrp->getTree();
				if (debug) {
					stdoutput.printf(
						"current query tree:\n");
					if (tree) {
						tree->getRootNode()->
							print(&stdoutput);
					}
					stdoutput.printf("\n");
				}
			}

			if (!tr->run(sqlrcon,sqlrcur,tree)) {
				return false;
			}

		} else {

			bool	freequery=false;
			if (tree) {
				if (sqlrp && !sqlrp->write(&tempquerystr)) {
					return false;
				}
				tree=NULL;
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

	if (tree) {
		if (sqlrp && !sqlrp->write(translatedquery)) {
			return false;
		}
	} else {
		translatedquery->append(query);
		if (sqlrp && sqlrp->parse(translatedquery->getString())) {
			tree=sqlrp->getTree();
		}
	}

	if (debug) {
		stdoutput.printf("\nquery tree after translation:\n");
		if (tree) {
			tree->getRootNode()->print(&stdoutput);
		}
		stdoutput.printf("\n");
	}

	return true;
}

void sqlrtranslations::endSession() {
	temptablepool->deallocate();
	tempindexpool->deallocate();
	temptablemap.clear();
	tempindexmap.clear();
}

xmldomnode *sqlrtranslations::newNode(xmldomnode *parentnode,
						const char *type) {
	xmldomnode	*retval=new xmldomnode(tree,
						parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,
						NULL,type,NULL);
	parentnode->appendChild(retval);
	return retval;
}

xmldomnode *sqlrtranslations::newNode(xmldomnode *parentnode,
				const char *type, const char *value) {
	xmldomnode	*node=newNode(parentnode,type);
	setAttribute(node,"value",value);
	return node;
}

xmldomnode *sqlrtranslations::newNodeAfter(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type) {
	xmldomnode	*retval=new xmldomnode(tree,
						parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,
						NULL,type,NULL);
	parentnode->insertChild(retval,node->getPosition()+1);
	return retval;
}

xmldomnode *sqlrtranslations::newNodeAfter(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type,
						const char *value) {
	xmldomnode	*retval=newNodeAfter(parentnode,node,type);
	setAttribute(retval,"value",value);
	return retval;
}

xmldomnode *sqlrtranslations::newNodeBefore(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type) {
	xmldomnode	*retval=new xmldomnode(tree,
						parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,
						NULL,type,NULL);
	parentnode->insertChild(retval,node->getPosition());
	return retval;
}

xmldomnode *sqlrtranslations::newNodeBefore(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type,
						const char *value) {
	xmldomnode	*retval=newNodeBefore(parentnode,node,type);
	setAttribute(retval,"value",value);
	return retval;
}

void sqlrtranslations::setAttribute(xmldomnode *node,
				const char *name, const char *value) {
	// FIXME: I shouldn't have to do this.
	// setAttribute should append it automatically
	if (node->getAttribute(name)!=node->getNullNode()) {
		node->setAttributeValue(name,value);
	} else {
		node->appendAttribute(name,value);
	}
}

bool sqlrtranslations::isString(const char *value) {
	size_t	length=charstring::length(value);
	return ((value[0]=='\'' && value[length-1]=='\'') ||
			(value[0]=='"' && value[length-1]=='"'));
}

bool	sqlrtranslations::getReplacementTableName(const char *database,
						const char *schema,
						const char *oldname,
						const char **newname) {
		return getReplacementName(&temptablemap,
						database,schema,
						oldname,newname);
}

bool	sqlrtranslations::getReplacementIndexName(const char *database,
						const char *schema,
						const char *oldname,
						const char **newname) {
		return getReplacementName(&tempindexmap,
						database,schema,
						oldname,newname);
}

bool	sqlrtranslations::getReplacementName(
				dictionary< databaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldname,
				const char **newname) {

	*newname=NULL;
	for (linkedlistnode< dictionarynode< databaseobject *, char * > *>
					*node=dict->getList()->getFirst();
						node; node=node->getNext()) {

		databaseobject	*dbo=node->getValue()->getKey();
		if (!charstring::compare(dbo->database,database) &&
			!charstring::compare(dbo->schema,schema) &&
			!charstring::compare(dbo->object,oldname)) {
			*newname=node->getValue()->getValue();
			return true;
		}
	}
	return false;
}

databaseobject *sqlrtranslations::createDatabaseObject(memorypool *pool,
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
	databaseobject	*dbo=
		(databaseobject *)pool->allocate(sizeof(databaseobject));


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

bool sqlrtranslations::removeReplacementTable(const char *database,
						const char *schema,
						const char *table) {

	// remove the table
	if (!removeReplacement(&temptablemap,database,schema,table)) {
		return false;
	}

	// remove any indices that depend on the table
	for (linkedlistnode< dictionarynode< databaseobject *, char * > *>
			*node=tempindexmap.getList()->getFirst(); node;) {

		databaseobject	*dbo=node->getValue()->getKey();

		// make sure to move on to the next node here rather than
		// after calling remove, otherwise it could cause a
		// reference-after-free condition
		node=node->getNext();

		if (!charstring::compare(dbo->database,database) &&
			!charstring::compare(dbo->schema,schema) &&
			!charstring::compare(dbo->dependency,table)) {

			tempindexmap.remove(dbo);
		}
	}
	return true;
}

bool sqlrtranslations::removeReplacementIndex(const char *database,
						const char *schema,
						const char *index) {
	return removeReplacement(&tempindexmap,database,schema,index);
}

bool sqlrtranslations::removeReplacement(
				dictionary< databaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *name) {

	for (linkedlistnode< dictionarynode< databaseobject *, char * > *>
				*node=dict->getList()->getFirst();
					node; node=node->getNext()) {

		databaseobject	*dbo=node->getValue()->getKey();
		const char	*replacementname=node->getValue()->getValue();
		if (!charstring::compare(dbo->database,database) &&
			!charstring::compare(dbo->schema,schema) &&
			!charstring::compare(replacementname,name)) {

			dict->remove(dbo);
			return true;
		}
	}
	return false;
}
