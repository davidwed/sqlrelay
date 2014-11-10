// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlreparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class uniquetodistinct : public sqlrtranslation {
	public:
			uniquetodistinct(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
	private:
		void	replaceUnique(xmldomnode *node);
};

uniquetodistinct::uniquetodistinct(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool uniquetodistinct::usesTree() {
	return true;
}

bool uniquetodistinct::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	replaceUnique(querytree->getRootNode());
	return true;
}

void uniquetodistinct::replaceUnique(xmldomnode *node) {

	// dig through child nodes
	for (xmldomnode *child=node->getFirstTagChild();
		!child->isNullNode(); child=child->getNextTagSibling()) {

		// look for "select" and "select into" nodes
		if (!charstring::compare(child->getName(),
						sqlreparser::_select) ||
			!charstring::compare(child->getName(),
						sqlreparser::_select_into)) {

			// look for a child node "unique"
			xmldomnode	*uniquenode=
				child->getFirstTagChild(sqlreparser::_unique);
			if (!uniquenode->isNullNode()) {

				// change it to "distinct"
				uniquenode->setName(sqlreparser::_distinct);
			}
		}

		// recurse
		replaceUnique(child);
	}
}

extern "C" {
	sqlrtranslation	*new_uniquetodistinct(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) {
		return new uniquetodistinct(sqlts,parameters,debug);
	}
}
