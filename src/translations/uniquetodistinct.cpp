// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqlrtranslation.h>
#include <debugprint.h>

class uniquetodistinct : public sqlrtranslation {
	public:
			uniquetodistinct(sqlrtranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		void	replaceUnique(xmldomnode *node);
};

uniquetodistinct::uniquetodistinct(sqlrtranslations *sqlts,
						xmldomnode *parameters) :
					sqlrtranslation(sqlts,parameters) {
}

bool uniquetodistinct::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
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
						sqlparser::_select) ||
			!charstring::compare(child->getName(),
						sqlparser::_select_into)) {

			// look for a child node "unique"
			xmldomnode	*uniquenode=
				child->getFirstTagChild(sqlparser::_unique);
			if (!uniquenode->isNullNode()) {

				// change it to "distinct"
				uniquenode->setName(sqlparser::_distinct);
			}
		}

		// recurse
		replaceUnique(child);
	}
}

extern "C" {
	sqlrtranslation	*new_uniquetodistinct(sqlrtranslations *sqlts,
						xmldomnode *parameters) {
		return new uniquetodistinct(sqlts,parameters);
	}
}
