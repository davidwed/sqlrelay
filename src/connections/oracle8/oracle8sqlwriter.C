#include <oracle8sqlwriter.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslatordebug.h>

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
	output->append("global temporary");
	return true;
}

bool oracle8sqlwriter::uniqueKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unique");
	return true;
}

bool oracle8sqlwriter::cascade(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("cascade constraints");
	return true;
}

bool oracle8sqlwriter::as(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// Oracle doesn't allow bind variables in the select clause of a
	// create table ... as select ... query.  So, if we find an "as" clause
	// then fake input binds for this set of queries.  It will go back to 
	// the default behavior when a new query is prepared.
	sqlrcur->setFakeInputBinds(true);
	return sqlwriter::as(node,output);
}

bool oracle8sqlwriter::isolationLevel(xmldomnode *node,
						stringbuffer *output) {
	debugFunction();
	output->append("isolation level ");
	// Oracle doesn't support "read uncommitted",
	// replace it with "read committed" which is close enough.
	// Oracle also doesn't support "repeatable read"
	// replace it with "serializable" which is close enough.
	const char	*value=node->getAttributeValue(sqlparser::_value);
	if (!charstring::compareIgnoringCase(value,"read uncommitted")) {
		output->append("read committed");
	} else if (!charstring::compareIgnoringCase(value,"repeatable read")) {
		output->append("serializable");
	} else {
		output->append(value);
	}
	return true;
}
