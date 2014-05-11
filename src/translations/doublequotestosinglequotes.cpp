// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqlrtranslation.h>
#include <debugprint.h>

class doublequotestosinglequotes : public sqlrtranslation {
	public:
			doublequotestosinglequotes(sqlrtranslations *sqlts,
							xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		bool	replaceDoubleQuotes(xmldomnode *node);
};

doublequotestosinglequotes::doublequotestosinglequotes(sqlrtranslations *sqlts,
					xmldomnode *parameters) :
					sqlrtranslation(sqlts,parameters) {
}

bool doublequotestosinglequotes::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	return replaceDoubleQuotes(querytree->getRootNode());
}

bool doublequotestosinglequotes::replaceDoubleQuotes(xmldomnode *node) {

	if (node->isNullNode()) {
		return true;
	}

	// get the node value and length
	const char	*value=node->getAttributeValue("value");
	size_t		valuelength=charstring::length(value);

	// if the node starts and ends with double quotes...
	if (!charstring::compare(node->getName(),sqlparser::_string_literal) &&
			valuelength>=2 &&
			*value=='"' && *(value+valuelength-1)=='"') {

		// build a new string surrounded by single quotes
		// unescaping escaped double quotes and escaping single quotes
		stringbuffer	newvalue;
		newvalue.append('\'');
		const char	*start=value+1;
		const char	*end=value+valuelength-2;
		for (const char *c=start; c<=end; c++) {
			if (c<end && *c=='"' && *(c+1)=='"') {
				c++;
			} else if (*c=='\'') {
				newvalue.append('\'');
			}
			newvalue.append(*c);
		}
		newvalue.append('\'');
		node->setAttributeValue("value",newvalue.getString());
	}

	// run on children, then next sibling
	return replaceDoubleQuotes(node->getFirstTagChild()) &&
		replaceDoubleQuotes(node->getNextTagSibling());
}

extern "C" {
	sqlrtranslation	*new_doublequotestosinglequotes(
					sqlrtranslations *sqlts,
					xmldomnode *parameters) {
		return new doublequotestosinglequotes(sqlts,parameters);
	}
}
