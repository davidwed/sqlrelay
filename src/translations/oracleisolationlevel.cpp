// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlreparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class oracleisolationlevel : public sqlrtranslation {
	public:
			oracleisolationlevel(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
	private:
		void	replaceIsolationLevel(xmldomnode *node);
};

oracleisolationlevel::oracleisolationlevel(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool oracleisolationlevel::usesTree() {
	return true;
}

bool oracleisolationlevel::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	replaceIsolationLevel(querytree->getRootNode());
	return true;
}

void oracleisolationlevel::replaceIsolationLevel(xmldomnode *node) {

	xmldomnode	*level=node->getFirstTagChild(sqlreparser::_set)->
				getFirstTagChild(sqlreparser::_transaction)->
				getFirstTagChild(sqlreparser::_isolation_level);
	if (level->isNullNode()) {
		return;
	}

	// Oracle doesn't support "read uncommitted",
	// replace it with "read committed" which is close enough.
	// Oracle also doesn't support "repeatable read)",
	// replace it with "serializable" which is close enough.
	const char	*value=level->getAttributeValue(sqlreparser::_value);
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
		level->setAttributeValue(sqlreparser::_value,"read committed");
	} else if (!charstring::compareIgnoringCase(value,"repeatable read") ||
		!charstring::compareIgnoringCase(value,"rr") ||
		!charstring::compareIgnoringCase(value,"2") ||
		!charstring::compareIgnoringCase(value,"snapshot") ||
		!charstring::compareIgnoringCase(value,"read stability") ||
		!charstring::compareIgnoringCase(value,"rs") ||
		!charstring::compareIgnoringCase(value,"3") ||
		!charstring::compareIgnoringCase(value,
					"snapshot table stability")) {
		level->setAttributeValue(sqlreparser::_value,"serializable");
	}
}

extern "C" {
	sqlrtranslation	*new_oracleisolationlevel(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new oracleisolationlevel(sqlts,parameters,debug);
	}
}
