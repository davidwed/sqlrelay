#include <oracle8sqlwriter.h>
#include <sqlrcursor.h>
#include <sqlelement.h>
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
		sqlelement::_on_commit,
		NULL
	};
	return additionalelements;
}

const char * const *oracle8sqlwriter::unsupportedElements() {
	debugFunction();
	static const char *unsupportedelements[]={
		// constraints...
		sqlelement::_unsigned,
		sqlelement::_zerofill,
		sqlelement::_binary,
		sqlelement::_character_set,
		sqlelement::_collate,
		sqlelement::_auto_increment,
		sqlelement::_key,
		sqlelement::_comment,
		sqlelement::_column_format,
		sqlelement::_match,
		sqlelement::_on_delete,
		sqlelement::_on_update,

		// drop...
		sqlelement::_drop_temporary,
		sqlelement::_restrict,

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
