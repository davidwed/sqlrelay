#include <oracle8sqltranslator.h>
#include <sqlelement.h>
#include <sqltranslatordebug.h>

oracle8sqltranslator::oracle8sqltranslator() : sqltranslator() {
	debugFunction();
}

oracle8sqltranslator::~oracle8sqltranslator() {
	debugFunction();
}

bool oracle8sqltranslator::applyRulesToQuery(xmldomnode *query) {
	debugFunction();

	for (xmldomnode *rule=rulesnode->getFirstTagChild();
		!rule->isNullNode(); rule=rule->getNextTagSibling()) {

		const char	*rulename=rule->getName();

		if (!charstring::compare(rulename,
				"temp_tables_preserve_rows_by_default")) {
			if (!tempTablesPreserveRowsByDefault(query,rule)) {
				return false;
			}
		}
	}

	return sqltranslator::applyRulesToQuery(query);
}

bool oracle8sqltranslator::nativizeDatatypes(xmldomnode *query,
						xmldomnode *rule) {
	debugFunction();
	return true;
}

bool oracle8sqltranslator::tempTablesPreserveRowsByDefault(
						xmldomnode *query,
						xmldomnode *rule) {
	debugFunction();

	// ignore non create-table queries
	xmldomnode	*table=
			query->getFirstTagChild(sqlelement::_create)->
				getFirstTagChild(sqlelement::_table);
	if (table->isNullNode()) {
		return true;
	}

	// ignore non-temporary tables
	xmldomnode	*temporary=
			query->getFirstTagChild(sqlelement::_create)->
				getFirstTagChild(sqlelement::_create_temporary);
	if (temporary->isNullNode()) {
		return true;
	}

	// if there's already an on commit clause then leave it alone
	xmldomnode	*oncommit=
			table->getFirstTagChild(sqlelement::_on_commit);
	if (!oncommit->isNullNode()) {
		return true;
	}

	// If no on commit clause has been declared then the app expects
	// the default behavior to be followed.  Override that by adding
	// an on commit clause which preserve rows...

	// find the columns clause
	xmldomnode	*columns=table->getFirstTagChild(sqlelement::_columns);

	// if there is no columns clause then bail, we've got bigger problems
	if (columns->isNullNode()) {
		return true;
	}
		
	// add a new on commit clause after the columns clause
	oncommit=newNodeAfter(table,columns,sqlelement::_on_commit);
	setAttribute(oncommit,sqlelement::_value,"preserve rows");
	return true;
}
