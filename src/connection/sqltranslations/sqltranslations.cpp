// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqltranslations.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <debugprint.h>

#include <rudiments/process.h>
#include <rudiments/character.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

sqltranslations::sqltranslations() {
	debugFunction();
	xmld=NULL;
	tree=NULL;
	temptablepool=new memorypool(0,128,100);
	tempindexpool=new memorypool(0,128,100);
}

sqltranslations::~sqltranslations() {
	debugFunction();
	unloadTranslations();
	delete xmld;
	delete temptablepool;
	delete tempindexpool;
}

bool sqltranslations::loadTranslations(const char *translations) {
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

void sqltranslations::unloadTranslations() {
	debugFunction();
	for (linkedlistnode< sqltranslationplugin * > *node=
						tlist.getFirstNode();
						node; node=node->getNext()) {
		sqltranslationplugin	*sqlt=node->getData();
		delete sqlt->tr;
		delete sqlt->dl;
		delete sqlt;
	}
	tlist.clear();
}

void sqltranslations::loadTranslation(xmldomnode *translation) {
	debugFunction();

	// ignore non-translations
	if (charstring::compare(translation->getName(),"translation")) {
		return;
	}

	// get the translation name
	const char	*module=translation->getAttributeValue("module");
	if (!charstring::length(module)) {
		return;
	}

	debugPrintf("loading translation: %s\n",module);

	// load the translation module
	stringbuffer	modulename;
	modulename.append(LIBEXECDIR);
	modulename.append("/sqltranslation_");
	modulename.append(module)->append(".so");
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		printf("failed to load translation module: %s\n",module);
		char	*error=dl->getError();
		printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the translation itself
	stringbuffer	functionname;
	functionname.append("new_")->append(module);
	sqltranslation *(*newTranslation)(sqltranslations *, xmldomnode *)=
			(sqltranslation *(*)(sqltranslations *, xmldomnode *))
				dl->getSymbol(functionname.getString());
	if (!newTranslation) {
		printf("failed to create translation: %s\n",module);
		char	*error=dl->getError();
		printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqltranslation	*tr=(*newTranslation)(this,translation);

	// add the plugin to the list
	sqltranslationplugin	*sqltp=new sqltranslationplugin;
	sqltp->tr=tr;
	sqltp->dl=dl;
	tlist.append(sqltp);
}

bool sqltranslations::runTranslations(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	if (!querytree) {
		return false;
	}

	tree=querytree;

	for (linkedlistnode< sqltranslationplugin * > *node=
						tlist.getFirstNode();
						node; node=node->getNext()) {
		if (!node->getData()->tr->run(sqlrcon,sqlrcur,querytree)) {
			return false;
		}
	}
	return true;
}

void sqltranslations::endSession() {
	temptablepool->free();
	tempindexpool->free();
	temptablemap.clear();
	tempindexmap.clear();
}

xmldomnode *sqltranslations::newNode(xmldomnode *parentnode, const char *type) {
	xmldomnode	*retval=new xmldomnode(tree,parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,type,NULL);
	parentnode->appendChild(retval);
	return retval;
}

xmldomnode *sqltranslations::newNode(xmldomnode *parentnode,
				const char *type, const char *value) {
	xmldomnode	*node=newNode(parentnode,type);
	setAttribute(node,sqlparser::_value,value);
	return node;
}

xmldomnode *sqltranslations::newNodeAfter(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type) {
	xmldomnode	*retval=new xmldomnode(tree,parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,type,NULL);
	parentnode->insertChild(retval,node->getPosition()+1);
	return retval;
}

xmldomnode *sqltranslations::newNodeAfter(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type,
						const char *value) {
	xmldomnode	*retval=newNodeAfter(parentnode,node,type);
	setAttribute(retval,sqlparser::_value,value);
	return retval;
}

xmldomnode *sqltranslations::newNodeBefore(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type) {
	xmldomnode	*retval=new xmldomnode(tree,parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,type,NULL);
	parentnode->insertChild(retval,node->getPosition());
	return retval;
}

xmldomnode *sqltranslations::newNodeBefore(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type,
						const char *value) {
	xmldomnode	*retval=newNodeBefore(parentnode,node,type);
	setAttribute(retval,sqlparser::_value,value);
	return retval;
}

void sqltranslations::setAttribute(xmldomnode *node,
				const char *name, const char *value) {
	// FIXME: I shouldn't have to do this.
	// setAttribute should append it automatically
	if (node->getAttribute(name)!=node->getNullNode()) {
		node->setAttributeValue(name,value);
	} else {
		node->appendAttribute(name,value);
	}
}

bool sqltranslations::isString(const char *value) {
	size_t	length=charstring::length(value);
	return ((value[0]=='\'' && value[length-1]=='\'') ||
			(value[0]=='"' && value[length-1]=='"'));
}

bool	sqltranslations::getReplacementTableName(const char *database,
						const char *schema,
						const char *oldname,
						const char **newname) {
		return getReplacementName(&temptablemap,
						database,schema,
						oldname,newname);
}

bool	sqltranslations::getReplacementIndexName(const char *database,
						const char *schema,
						const char *oldname,
						const char **newname) {
		return getReplacementName(&tempindexmap,
						database,schema,
						oldname,newname);
}

bool	sqltranslations::getReplacementName(
				dictionary< databaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldname,
				const char **newname) {

	*newname=NULL;
	for (dictionarylistnode< databaseobject *, char * > *node=
					dict->getList()->getFirstNode();
		node;
		node=(dictionarylistnode< databaseobject *, char *> *)
							node->getNext()) {

		databaseobject	*dbo=node->getData()->getKey();
		if (!charstring::compare(dbo->database,database) &&
			!charstring::compare(dbo->schema,schema) &&
			!charstring::compare(dbo->object,oldname)) {
			*newname=node->getData()->getData();
			return true;
		}
	}
	return false;
}

databaseobject *sqltranslations::createDatabaseObject(memorypool *pool,
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
		databasecopy=(char *)pool->malloc(
				charstring::length(database)+1);
		charstring::copy(databasecopy,database);
	}
	if (schema) {
		schemacopy=(char *)pool->malloc(
				charstring::length(schema)+1);
		charstring::copy(schemacopy,schema);
	}
	if (object) {
		objectcopy=(char *)pool->malloc(
				charstring::length(object)+1);
		charstring::copy(objectcopy,object);
	}
	if (dependency) {
		dependencycopy=(char *)pool->malloc(
				charstring::length(dependency)+1);
		charstring::copy(dependencycopy,dependency);
	}

	// create the databaseobject
	databaseobject	*dbo=
		(databaseobject *)pool->malloc(sizeof(databaseobject));


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

bool sqltranslations::removeReplacementTable(const char *database,
						const char *schema,
						const char *table) {

	// remove the table
	if (!removeReplacement(&temptablemap,database,schema,table)) {
		return false;
	}

	// remove any indices that depend on the table
	for (dictionarylistnode< databaseobject *, char * > *node=
					tempindexmap.getList()->getFirstNode();
		node;
		node=(dictionarylistnode< databaseobject *, char *> *)
							node->getNext()) {

		databaseobject	*dbo=node->getData()->getKey();
		if (!charstring::compare(dbo->database,database) &&
			!charstring::compare(dbo->schema,schema) &&
			!charstring::compare(dbo->dependency,table)) {

			tempindexmap.removeData(dbo);
		}
	}
	return true;
}

bool sqltranslations::removeReplacementIndex(const char *database,
						const char *schema,
						const char *index) {
	return removeReplacement(&tempindexmap,database,schema,index);
}

bool sqltranslations::removeReplacement(
				dictionary< databaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *name) {

	for (dictionarylistnode< databaseobject *, char * > *node=
					dict->getList()->getFirstNode();
		node;
		node=(dictionarylistnode< databaseobject *, char *> *)
							node->getNext()) {

		databaseobject	*dbo=node->getData()->getKey();
		const char	*replacementname=node->getData()->getData();
		if (!charstring::compare(dbo->database,database) &&
			!charstring::compare(dbo->schema,schema) &&
			!charstring::compare(replacementname,name)) {

			dict->removeData(dbo);
			return true;
		}
	}
	return false;
}
