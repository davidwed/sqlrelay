// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <rudiments/character.h>
#include <rudiments/charstring.h>
#include <rudiments/stringbuffer.h>
#include <sqlparser.h>
#include <debugprint.h>

sqlparser::sqlparser() {
	tree=NULL;
	error=false;
}

sqlparser::~sqlparser() {
	delete tree;
}

xmldom *sqlparser::getTree() {
	return tree;
}

xmldom *sqlparser::detachTree() {
	xmldom	*retval=tree;
	tree=NULL;
	return retval;
}

xmldomnode *sqlparser::newNode(xmldomnode *parentnode, const char *type) {
	xmldomnode	*retval=new xmldomnode(tree,parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,type,NULL);
	parentnode->appendChild(retval);
	return retval;
}

xmldomnode *sqlparser::newNode(xmldomnode *parentnode,
				const char *type, const char *value) {
	xmldomnode	*node=newNode(parentnode,type);
	setAttribute(node,_value,value);
	return node;
}

void sqlparser::setAttribute(xmldomnode *node,
				const char *name, const char *value) {
	// FIXME: I shouldn't have to do this.
	// setAttribute should append it automatically
	if (node->getAttribute(name)!=node->getNullNode()) {
		node->setAttributeValue(name,value);
	} else {
		node->appendAttribute(name,value);
	}
}

char *sqlparser::cleanQuery(const char *query) {

	// remove comments and convert all whitespace into spaces
	stringbuffer	cleanquery;
	bool		whitespace=false;
	const char	*ptr=query;
	char		prevchar='\0';
	for (;;) {

		// skip comments
		if (!charstring::compare(ptr,"-- ",3)) {
			while (*ptr && *ptr!='\n') {
				ptr++;
			}
			if (*ptr) {
				ptr++;
			}
		}

		// skip quoted strings
		if (*ptr=='\'') {
			do {
				cleanquery.append(*ptr);
				ptr++;
			} while (*ptr && *ptr!='\'');
			// NOTE: the trailing ' will be appended by code below
		}

		// skip double-quoted strings
		if (*ptr=='"') {
			do {
				cleanquery.append(*ptr);
				ptr++;
			} while (*ptr && *ptr!='"');
			// NOTE: the trailing " will be appended by code below
		}

		// convert whitespace into spaces
		if (character::isWhitespace(*ptr)) {
			cleanquery.append(' ');
		} else if (!*ptr) {
			break;
		} else {
			cleanquery.append(*ptr);
			prevchar=*ptr;
			whitespace=false;
		}

		// move on to the next character
		ptr++;
	}
	debugPrintf("clean query:\n\"%s\"\n\n",cleanquery.getString());

	char	*retval=cleanquery.detachString();

	// trim any lingering leading/trailing whitespace
	charstring::bothTrim(retval);

	// return the clean query string
	debugPrintf("clean and fixed-up query:\n\"%s\"\n\n",retval);
	return retval;
}
