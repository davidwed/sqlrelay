// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlreparser.h>
#include <sqlrelay/sqlrserver.h>
#include <debugprint.h>

class show : public sqlrtranslation {
	public:
			show(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
};

show::show(sqlrtranslations *sqlts, xmldomnode *parameters, bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool show::usesTree() {
	return true;
}

bool show::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {

	xmldomnode	*node=querytree->getRootNode()->getFirstTagChild();

	if (node->isNullNode()) {
		return true;
	}

	if (!charstring::compare(node->getName(),sqlreparser::_show)) {

		const char	*value=node->getAttributeValue(
						sqlreparser::_value);

		if (!charstring::compare(value,"client_encoding")) {
			
			querytree->getRootNode()->deleteChild(node);

			stringbuffer	query;
			query.append("select '");
			query.append(parameters->
					getAttributeValue("client_encoding"));
			query.append("' from dual");

			sqlreparser	sqlp;
			sqlp.useTree(querytree);
			sqlp.parse(query.getString());
		}
	}
	return true;
}

extern "C" {
	sqlrtranslation	*new_show(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new show(sqlts,parameters,debug);
	}
}
