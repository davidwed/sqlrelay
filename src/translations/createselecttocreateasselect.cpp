// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslation.h>
#include <debugprint.h>

class createselecttocreateasselect : public sqltranslation {
	public:
			createselecttocreateasselect(
						sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldom *querytree);
	private:
		void	insertAs(xmldomnode *node);
};

createselecttocreateasselect::createselecttocreateasselect(
						sqltranslations *sqlts,
						xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool createselecttocreateasselect::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	insertAs(querytree->getRootNode());
	return true;
}

void createselecttocreateasselect::insertAs(xmldomnode *node) {

	xmldomnode	*select=node->getFirstTagChild(sqlparser::_create)->
					getFirstTagChild(sqlparser::_table)->
					getFirstTagChild(sqlparser::_select);
	if (select->isNullNode()) {
		return;
	}

	if (select->getPreviousTagSibling(sqlparser::_as)->isNullNode()) {
		select->getParent()->insertTag(sqlparser::_as,
						select->getPosition());
	}
}

extern "C" {
	sqltranslation	*new_createselecttocreateasselect(
					sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new createselecttocreateasselect(sqlts,parameters);
	}
}
