// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqltranslations.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslations/translatedatetimes.h>
#include <sqltranslations/temptableslocalize.h>
#include <sqltranslations/locksnowaitbydefault.h>
#include <sqltranslations/oracletemptablespreserverowsbydefault.h>
#include <sqltranslations/temptablesaddmissingcolumns.h>
#include <sqltranslations/doublequotestosinglequotes.h>
#include <sqltranslations/informixtooracledate.h>
#include <debugprint.h>

#include <rudiments/process.h>
#include <rudiments/character.h>

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
		sqltranslation	*tr=loadTranslation(translation);
		if (tr) {
			tlist.append(tr);
		}
	}

	return true;
}

void sqltranslations::unloadTranslations() {
	debugFunction();
	for (linkedlistnode< sqltranslation * > *node=tlist.getFirstNode();
						node; node=node->getNext()) {
		delete node->getData();
	}
	tlist.clear();
}

sqltranslation *sqltranslations::loadTranslation(xmldomnode *translation) {
	debugFunction();

	// ignore non-translations
	if (charstring::compare(translation->getName(),"translation")) {
		return NULL;
	}

	// ultimatelty this should dynamically load plugins,
	// but for now it's static...

	// get the translation name
	const char	*file=translation->getAttributeValue("file");
	if (!charstring::length(file)) {
		return NULL;
	}

	debugPrintf("loading translation: %s\n",file);

	// load the translation itself
	if (!charstring::compare(file,"translatedatetimes")) {
		return new translatedatetimes(this,translation);
	} else if (!charstring::compare(file,"temptableslocalize")) {
		return new temptableslocalize(this,translation);
	} else if (!charstring::compare(file,"locksnowaitbydefault")) {
		return new locksnowaitbydefault(this,translation);
	} else if (!charstring::compare(file,"temptablesaddmissingcolumns")) {
		return new temptablesaddmissingcolumns(this,translation);
	} else if (!charstring::compare(file,
			"oracletemptablespreserverowsbydefault")) {
		return new oracletemptablespreserverowsbydefault(
							this,translation);
	} else if (!charstring::compare(file,"doublequotestosinglequotes")) {
		return new doublequotestosinglequotes(this,translation);
	} else if (!charstring::compare(file,"informixtooracledate")) {
		return new informixtooracledate(this,translation);
	}
	return NULL;
}

bool sqltranslations::runTranslations(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	if (!querytree) {
		return false;
	}

	tree=querytree;

	for (linkedlistnode< sqltranslation * > *node=tlist.getFirstNode();
						node; node=node->getNext()) {
		if (!node->getData()->run(sqlrcon,sqlrcur,querytree)) {
			return false;
		}
	}
	return true;
}

void sqltranslations::endSession() {
	temptablepool->free();
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

	// find the position after "node"
	uint64_t	position=1;
	for (xmldomnode *child=parentnode->getChild((uint64_t)0);
		!child->isNullNode(); child=child->getNextSibling()) {
		if (child==node) {
			break;
		}
		position++;
	}

	// create a new node and insert it at that position
	xmldomnode	*retval=new xmldomnode(tree,parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,type,NULL);
	parentnode->insertChild(retval,position);
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

	// find the position after "node"
	uint64_t	position=0;
	for (xmldomnode *child=parentnode->getChild((uint64_t)0);
		!child->isNullNode(); child=child->getNextSibling()) {
		if (child==node) {
			break;
		}
		position++;
	}

	// create a new node and insert it at that position
	xmldomnode	*retval=new xmldomnode(tree,parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,type,NULL);
	parentnode->insertChild(retval,position);
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

bool	sqltranslations::getReplacementTableName(const char *oldname,
						const char **newname) {
	return temptablemap.getData((char *)oldname,(char **)newname);
}

bool	sqltranslations::getReplacementIndexName(const char *oldname,
						const char **newname) {
	return tempindexmap.getData((char *)oldname,(char **)newname);
}
