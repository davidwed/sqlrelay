// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlreparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class createselecttocreateasselect : public sqlrtranslation {
	public:
			createselecttocreateasselect(
						sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldom *querytree);
	private:
		void	insertAs(xmldomnode *node);
};

createselecttocreateasselect::createselecttocreateasselect(
						sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool createselecttocreateasselect::usesTree() {
	return true;
}

bool createselecttocreateasselect::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	insertAs(querytree->getRootNode());
	return true;
}

void createselecttocreateasselect::insertAs(xmldomnode *node) {

	xmldomnode	*select=node->getFirstTagChild(sqlreparser::_create)->
					getFirstTagChild(sqlreparser::_table)->
					getFirstTagChild(sqlreparser::_select);
	if (select->isNullNode()) {
		return;
	}

	if (select->getPreviousTagSibling(sqlreparser::_as)->isNullNode()) {
		select->getParent()->insertTag(sqlreparser::_as,
						select->getPosition());
	}
}

extern "C" {
	sqlrtranslation	*new_createselecttocreateasselect(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new createselecttocreateasselect(sqlts,parameters,debug);
	}
}
