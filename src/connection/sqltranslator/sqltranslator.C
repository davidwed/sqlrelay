// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqltranslator.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslatordebug.h>

sqltranslator::sqltranslator() {
	debugFunction();
	xmld=NULL;
	tree=NULL;
	sqlrcon=NULL;
	sqlrcur=NULL;
}

sqltranslator::~sqltranslator() {
	debugFunction();
	delete xmld;
}

bool sqltranslator::loadRules(const char *rules) {
	debugFunction();
	delete xmld;
	xmld=new xmldom();
	if (xmld->parseString(rules)) {
		rulesnode=xmld->getRootNode()->
				getFirstTagChild("sqltranslationrules");
		return !rulesnode->isNullNode();
	}
	return false;
}

bool sqltranslator::applyRules(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	this->sqlrcon=sqlrcon;
	this->sqlrcur=sqlrcur;
	tree=querytree;
	return applyRulesToQuery(querytree->getRootNode());
}

bool sqltranslator::applyRulesToQuery(xmldomnode *query) {
	debugFunction();

	for (xmldomnode *rule=rulesnode->getFirstTagChild();
		!rule->isNullNode(); rule=rule->getNextTagSibling()) {

		const char	*rulename=rule->getName();

		if (!charstring::compare(rulename,
					"nativize_datatypes")) {
			if (!nativizeDatatypes(query,rule)) {
				return false;
			}
		} else if (!charstring::compare(rulename,
					"convert_datatypes")) {
			if (!convertDatatypes(query,rule)) {
				return false;
			}
		} else if (!charstring::compare(rulename,
				"trim_columns_compared_to_string_binds")) {
			if (!trimColumnsComparedToStringBinds(query,rule)) {
				return false;
			}
		}
	}
	return true;
}

bool sqltranslator::nativizeDatatypes(xmldomnode *query, xmldomnode *rule) {
	debugFunction();
	// by default, do nothing
	return true;
}

bool sqltranslator::convertDatatypes(xmldomnode *query, xmldomnode *rule) {
	debugFunction();
	return true;
}

bool sqltranslator::trimColumnsComparedToStringBinds(xmldomnode *query,
							xmldomnode *rule) {
	debugFunction();
	return true;
}

xmldomnode *sqltranslator::newNode(xmldomnode *parentnode, const char *type) {
	xmldomnode	*retval=new xmldomnode(tree,parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,type,NULL);
	parentnode->appendChild(retval);
	return retval;
}

xmldomnode *sqltranslator::newNode(xmldomnode *parentnode,
				const char *type, const char *value) {
	xmldomnode	*node=newNode(parentnode,type);
	setAttribute(node,sqlparser::_value,value);
	return node;
}

xmldomnode *sqltranslator::newNodeAfter(xmldomnode *parentnode,
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

xmldomnode *sqltranslator::newNodeAfter(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type,
						const char *value) {
	xmldomnode	*retval=newNodeAfter(parentnode,node,type);
	setAttribute(retval,sqlparser::_value,value);
	return retval;
}

void sqltranslator::setAttribute(xmldomnode *node,
				const char *name, const char *value) {
	// FIXME: I shouldn't have to do this.
	// setAttribute should append it automatically
	if (node->getAttribute(name)!=node->getNullNode()) {
		node->setAttributeValue(name,value);
	} else {
		node->appendAttribute(name,value);
	}
}
