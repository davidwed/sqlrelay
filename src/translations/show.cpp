// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslation.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class show : public sqltranslation {
	public:
			show(sqltranslations *sqlts,
					xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
};

show::show(sqltranslations *sqlts,xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool show::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {

	xmldomnode	*node=querytree->getRootNode()->getFirstTagChild();

	if (node->isNullNode()) {
		return true;
	}

	if (!charstring::compare(node->getName(),sqlparser::_show)) {

		const char	*value=node->getAttributeValue(
						sqlparser::_value);

		if (!charstring::compare(value,"client_encoding")) {
			
			querytree->getRootNode()->deleteChild(node);

			stringbuffer	query;
			query.append("select '");
			query.append(parameters->
					getAttributeValue("client_encoding"));
			query.append("' from dual");

			sqlparser	sqlp;
			sqlp.useTree(querytree);
			sqlp.parse(query.getString());
		}
	}
	return true;
}

extern "C" {
	sqltranslation	*new_show(sqltranslations *sqlts,
						xmldomnode *parameters) {
		return new show(sqlts,parameters);
	}
}
