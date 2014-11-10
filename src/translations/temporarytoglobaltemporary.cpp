// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlreparser.h>
#include <sqlrelay/sqlrserver.h>
#include <debugprint.h>

class temporarytoglobaltemporary : public sqlrtranslation {
	public:
			temporarytoglobaltemporary(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
	private:
		void	insertGlobal(xmldomnode *node);
};

temporarytoglobaltemporary::temporarytoglobaltemporary(
						sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool temporarytoglobaltemporary::usesTree() {
	return true;
}

bool temporarytoglobaltemporary::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	insertGlobal(querytree->getRootNode());
	return true;
}

void temporarytoglobaltemporary::insertGlobal(xmldomnode *node) {

	xmldomnode	*create=
			node->getFirstTagChild(sqlreparser::_create);
	if (create->isNullNode()) {
		return;
	}

	xmldomnode	*temporary=
			create->getFirstTagChild(sqlreparser::_temporary);
	if (temporary->isNullNode()) {
		return;
	}

	if (temporary->getPreviousTagSibling(sqlreparser::_global)->
							isNullNode()) {
		create->insertTag(sqlreparser::_global,temporary->getPosition());
	}
}

extern "C" {
	sqlrtranslation	*new_temporarytoglobaltemporary(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new temporarytoglobaltemporary(sqlts,parameters,debug);
	}
}
