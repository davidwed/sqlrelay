// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class mysqlunsupported : public sqlrtranslation {
	public:
			mysqlunsupported(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
	private:
		void	removeNoWait(xmldomnode *node);
};

mysqlunsupported::mysqlunsupported(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool mysqlunsupported::usesTree() {
	return true;
}

bool mysqlunsupported::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
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
