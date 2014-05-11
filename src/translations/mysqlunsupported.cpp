// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqlrtranslation.h>
#include <debugprint.h>

class mysqlunsupported : public sqlrtranslation {
	public:
			mysqlunsupported(sqlrtranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		void	removeNoWait(xmldomnode *node);
};

mysqlunsupported::mysqlunsupported(sqlrtranslations *sqlts,
						xmldomnode *parameters) :
					sqlrtranslation(sqlts,parameters) {
}

bool mysqlunsupported::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	removeNoWait(querytree->getRootNode());
	return true;
}

void mysqlunsupported::removeNoWait(xmldomnode *node) {

	if (!charstring::compare(node->getName(),sqlparser::_nowait)) {
		node->setAttributeValue("supported","false");
		return;
	}

	xmldomnode *child=node->getFirstTagChild();
	while (!child->isNullNode()) {
		xmldomnode	*next=child->getNextTagSibling();
		removeNoWait(child);
		child=next;
	}
}

extern "C" {
	sqlrtranslation	*new_mysqlunsupported(sqlrtranslations *sqlts,
					xmldomnode *parameters) {
		return new mysqlunsupported(sqlts,parameters);
	}
}