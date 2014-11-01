// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class mysqlunsupported : public sqlrtranslation {
	public:
			mysqlunsupported(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		void	removeNoWait(xmldomnode *node);
};

mysqlunsupported::mysqlunsupported(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
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
	sqlrtranslation	*new_mysqlunsupported(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new mysqlunsupported(sqlts,parameters,debug);
	}
}
