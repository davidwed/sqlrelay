// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class temporarytoglobaltemporary : public sqlrtranslation {
	public:
			temporarytoglobaltemporary(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
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

bool temporarytoglobaltemporary::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	insertGlobal(querytree->getRootNode());
	return true;
}

void temporarytoglobaltemporary::insertGlobal(xmldomnode *node) {

	xmldomnode	*create=
			node->getFirstTagChild(sqlparser::_create);
	if (create->isNullNode()) {
		return;
	}

	xmldomnode	*temporary=
			create->getFirstTagChild(sqlparser::_temporary);
	if (temporary->isNullNode()) {
		return;
	}

	if (temporary->getPreviousTagSibling(sqlparser::_global)->
							isNullNode()) {
		create->insertTag(sqlparser::_global,temporary->getPosition());
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
