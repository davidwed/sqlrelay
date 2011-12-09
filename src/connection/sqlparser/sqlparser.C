// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <rudiments/charstring.h>
#include <rudiments/stringbuffer.h>
#include <sqlparser.h>
#include <sqlelement.h>
#include <sqltranslatordebug.h>

sqlparser::sqlparser() {
	tree=NULL;
}

sqlparser::~sqlparser() {
	delete tree;
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
	setAttribute(node,sqlelement::value,value);
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

	// remove comments and compress whitespace...
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

		// compress whitespace
		if (*ptr==' ' || *ptr=='\t' || *ptr=='\n' || *ptr=='\r') {
			if (!whitespace) {
				// append whitespace unless the previous
				// character was an open paren, comma, or
				// operator or the next character is a
				// closed paren, comma or operator
				if (!inSet(prevchar,"(,~!^*-+=<>/&|") &&
					!inSet(*(ptr+1),"),~!^*-+=<>/&|")) {
					cleanquery.append(' ');
				}
				whitespace=true;
			}
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
	debugPrintf("clean query:\n\"%s\"\n",cleanquery.getString());

	// A side effect of the above is that it removes spaces surrounding
	// *'s that are part of a select expression.  This could confound
	// parsing later.  Run back through and fix those up...

	// another buffer
	stringbuffer	fixedquery;

	// initialize a pointer to run through the string with
	ptr=cleanquery.getString();

	// quote flags
	bool	inquotes=false;
	bool	indoublequotes=false;

	// in/out of select flag
	// this is checked here because later on in the query,
	// the select will be preceeded by a space or left paren
	bool	inselect=!charstring::compareIgnoringCase(ptr,"select ",7) ||
			!charstring::compareIgnoringCase(ptr,"select*",7);

	// run through the clean query
	for (;;) {

		// break on end of string
		if (!*ptr) {
			break;
		}

		// check for quotes or doublequotes
		if (!indoublequotes && *ptr=='\'') {
			inquotes=!inquotes;
		} else if (!inquotes && *ptr=='"') {
			indoublequotes=!indoublequotes;
		}

		// check for the beginning of a select clause
		if (!inquotes && !indoublequotes &&
			(!charstring::compareIgnoringCase(ptr," select ",8) ||
			!charstring::compareIgnoringCase(ptr," select*",8) ||
			!charstring::compareIgnoringCase(ptr,"(select*",8) ||
			!charstring::compareIgnoringCase(ptr,"(select ",8))) {
			inselect=true;
		}

		// here are the cases that need attention
		bool	starfrom=!charstring::compareIgnoringCase(
							ptr,"*from ",6);
		if (inselect &&
			(!charstring::compareIgnoringCase(ptr,"*,",2) ||
			starfrom)) {

			// if we hit a *from then we're not in a select anymore
			if (starfrom) {
				inselect=false;
			}

			// prepend a space unless
			// there was a . or , before the *
			if (*(ptr-1)!='.' && *(ptr-1)!=',') {
				fixedquery.append(' ');
			}

			// append the * and a space
			fixedquery.append("*");

			// append a space unless the next character is a comma
			if (*(ptr+1)!=',') {
				fixedquery.append(' ');
			}

			// bump the pointer forward onto the "from"
			ptr++;
		}

		// check for the end of the expresssion part of a select clause
		if (inselect &&
			!charstring::compareIgnoringCase(ptr," from ",6)) {
			inselect=false;
		}

		// append the character
		fixedquery.append(*ptr);

		// move on
		ptr++;
	}

	// detach the fixed-up query string
	char	*retval=fixedquery.detachString();

	// trim any lingering leading/trailing whitespace
	charstring::bothTrim(retval);

	// return the clean query string
	debugPrintf("clean and fixed-up query:\n\"%s\"\n",retval);
	return retval;
}

bool sqlparser::inSet(char c, const char *set) {
	for (uint16_t i=0; set[i]; i++) {
		if (set[i]==c) {
			return true;
		}
	}
	return false;
}
