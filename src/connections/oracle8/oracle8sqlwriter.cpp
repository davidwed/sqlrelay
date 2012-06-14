#include <oracle8sqlwriter.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <debugprint.h>

oracle8sqlwriter::oracle8sqlwriter() : sqlwriter() {
	debugFunction();
}

oracle8sqlwriter::~oracle8sqlwriter() {
	debugFunction();
}

const char * const *oracle8sqlwriter::additionalElements() {
	debugFunction();
	static const char *additionalelements[]={
		// on commit...
		sqlparser::_on_commit,
		sqlparser::_as,
		NULL
	};
	return additionalelements;
}

const char * const *oracle8sqlwriter::unsupportedElements() {
	debugFunction();
	static const char *unsupportedelements[]={
		// constraints...
		sqlparser::_unsigned,
		sqlparser::_zerofill,
		sqlparser::_binary,
		sqlparser::_character_set,
		sqlparser::_collate,
		sqlparser::_auto_increment,
		sqlparser::_key,
		sqlparser::_comment,
		sqlparser::_column_format,
		sqlparser::_match,
		sqlparser::_on_delete,
		sqlparser::_on_update,

		// drop...
		sqlparser::_drop_temporary,
		sqlparser::_restrict,

		// set...
		sqlparser::_set_global,
		sqlparser::_set_session,

		NULL
	};
	return unsupportedelements;
}

bool oracle8sqlwriter::temporary(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (node->getPreviousTagSibling("global")->isNullNode()) {
		output->append("global ");
	}
	output->append("temporary");
	return true;
}

bool oracle8sqlwriter::uniqueKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unique");
	return true;
}

bool oracle8sqlwriter::cascade(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("cascade");
	if (node->getFirstTagChild(
			sqlparser::_cascade_constraints_clause)->isNullNode()) {
		output->append(" constraints");
	}
	return true;
}

bool oracle8sqlwriter::selectQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// if the select is a clause of a create table command then the
	// as keyword must preceed it.
	if (!charstring::compare(node->getParent()->getName(),
						sqlparser::_table) &&
		charstring::compare(node->getPreviousSibling()->getName(),
							sqlparser::_as) &&
		charstring::compare(node->getPreviousSibling()->getName(),
							sqlparser::_union)) {
		if (!as(node,output)) {
			return false;
		}
	}
	return sqlwriter::selectQuery(node,output);
}

bool oracle8sqlwriter::as(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// Oracle doesn't allow bind variables in the select clause of a
	// create table ... as select ... query.  So, if we find an "as" clause
	// then fake input binds for this set of queries.  It will go back to 
	// the default behavior when a new query is prepared.
	sqlrcur->setFakeInputBindsForThisQuery(true);
	return sqlwriter::as(node,output);
}

bool oracle8sqlwriter::isolationLevel(xmldomnode *node,
						stringbuffer *output) {
	debugFunction();
	output->append("isolation level ");
	// Oracle doesn't support "read uncommitted",
	// replace it with "read committed" which is close enough.
	// Oracle also doesn't support "repeatable read)",
	// replace it with "serializable" which is close enough.
	const char	*value=node->getAttributeValue(sqlparser::_value);
	if (!charstring::compareIgnoringCase(value,"read uncommitted") || 
		!charstring::compareIgnoringCase(value,"ur") ||
		!charstring::compareIgnoringCase(value,"0") ||
		!charstring::compareIgnoringCase(value,
					"read committed no record version") ||
		!charstring::compareIgnoringCase(value,"cursor stability") ||
		!charstring::compareIgnoringCase(value,"cs") ||
		!charstring::compareIgnoringCase(value,"1") ||
		!charstring::compareIgnoringCase(value,
					"read committed record vresion")) {
		output->append("read committed");
	} else if (!charstring::compareIgnoringCase(value,"repeatable read") ||
		!charstring::compareIgnoringCase(value,"rr") ||
		!charstring::compareIgnoringCase(value,"2") ||
		!charstring::compareIgnoringCase(value,"snapshot") ||
		!charstring::compareIgnoringCase(value,"read stability") ||
		!charstring::compareIgnoringCase(value,"rs") ||
		!charstring::compareIgnoringCase(value,"3") ||
		!charstring::compareIgnoringCase(value,
					"snapshot table stability")) {
		output->append("serializable");
	} else {
		output->append(value);
	}
	return true;
}
