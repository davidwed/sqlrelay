// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslation.h>
#include <debugprint.h>

class temporarytoglobaltemporary : public sqltranslation {
	public:
			temporarytoglobaltemporary(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		void	insertGlobal(xmldomnode *node);
};

temporarytoglobaltemporary::temporarytoglobaltemporary(sqltranslations *sqlts,
						xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool temporarytoglobaltemporary::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	insertGlobal(querytree->getRootNode());
	return true;
}

void temporarytoglobaltemporary::insertGlobal(xmldomnode *node) {

	xmldomnode	*create=node->getFirstTagChild("create");
	if (create->isNullNode()) {
		return;
	}

	xmldomnode	*temporary=create->getFirstTagChild("temporary");
	if (temporary->isNullNode()) {
		return;
	}

	if (temporary->getPreviousTagSibling("global")->isNullNode()) {
		create->insertTag("global",temporary->getPosition());
	}
}

extern "C" {
	sqltranslation	*new_temporarytoglobaltemporary(sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new temporarytoglobaltemporary(sqlts,parameters);
	}
}
