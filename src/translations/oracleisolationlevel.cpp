// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqlrtranslation.h>
#include <debugprint.h>

class oracleisolationlevel : public sqlrtranslation {
	public:
			oracleisolationlevel(sqlrtranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		void	replaceIsolationLevel(xmldomnode *node);
};

oracleisolationlevel::oracleisolationlevel(sqlrtranslations *sqlts,
						xmldomnode *parameters) :
					sqlrtranslation(sqlts,parameters) {
}

bool oracleisolationlevel::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	replaceIsolationLevel(querytree->getRootNode());
	return true;
}

void oracleisolationlevel::replaceIsolationLevel(xmldomnode *node) {

	xmldomnode	*level=node->getFirstTagChild(sqlparser::_set)->
				getFirstTagChild(sqlparser::_transaction)->
				getFirstTagChild(sqlparser::_isolation_level);
	if (level->isNullNode()) {
		return;
	}

	// Oracle doesn't support "read uncommitted",
	// replace it with "read committed" which is close enough.
	// Oracle also doesn't support "repeatable read)",
	// replace it with "serializable" which is close enough.
	const char	*value=level->getAttributeValue(sqlparser::_value);
	if (!charstring::compareIgnoringCase(value,"read uncommitted") || 
		!charstring::compareIgnoringCase(value,"ur") ||
		!charstring::compareIgnoringCase(value,"0") ||
		!charstring::compareIgnoringCase(value,
					"read committed no record version") ||
		!charstring::compareIgnoringCase(value,"cursor stability") ||
		!charstring::compareIgnoringCase(value,"cs") ||
		!charstring::compareIgnoringCase(value,"1") ||
		!charstring::compareIgnoringCase(value,
					"read committed record version")) {
		level->setAttributeValue(sqlparser::_value,"read committed");
	} else if (!charstring::compareIgnoringCase(value,"repeatable read") ||
		!charstring::compareIgnoringCase(value,"rr") ||
		!charstring::compareIgnoringCase(value,"2") ||
		!charstring::compareIgnoringCase(value,"snapshot") ||
		!charstring::compareIgnoringCase(value,"read stability") ||
		!charstring::compareIgnoringCase(value,"rs") ||
		!charstring::compareIgnoringCase(value,"3") ||
		!charstring::compareIgnoringCase(value,
					"snapshot table stability")) {
		level->setAttributeValue(sqlparser::_value,"serializable");
	}
}

extern "C" {
	sqlrtranslation	*new_oracleisolationlevel(
					sqlrtranslations *sqlts,
					xmldomnode *parameters) {
		return new oracleisolationlevel(sqlts,parameters);
	}
}