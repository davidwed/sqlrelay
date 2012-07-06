// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltranslations/extendtooracletodate.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>

extendtooracletodate::extendtooracletodate(sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool extendtooracletodate::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {

	return translateExtends(sqlrcon,sqlrcur,querytree->getRootNode());
}

bool extendtooracletodate::translateExtends(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {
	debugFunction();

	// look for extend function
	if (!charstring::compare(
			node->getName(),sqlparser::_function) &&
		!charstring::compareIgnoringCase(
			node->getAttributeValue(sqlparser::_value),
			"extend")) {

		// translate it
		if (!translateExtend(sqlrcon,sqlrcur,node)) {
			return false;
		}
	}

	// convert child nodes...
	for (node=node->getFirstTagChild();
			!node->isNullNode(); node=node->getNextTagSibling()) {
		if (!translateExtends(sqlrcon,sqlrcur,node)) {
			return false;
		}
	}
	return true;
}

enum timeparts_t {
	TIMEPARTS_YEAR=0,
	TIMEPARTS_MONTH,
	TIMEPARTS_DAY,
	TIMEPARTS_HOUR,
	TIMEPARTS_MINUTE,
	TIMEPARTS_SECOND,
	TIMEPARTS_FRACTION,
	TIMEPARTS_NULL
};

static const char *timeparts[]={
	"year",
	"month",
	"day",
	"hour",
	"minute",
	"second",
	"fraction",
	NULL
};

bool extendtooracletodate::translateExtend(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {
	debugFunction();

	// get the function node
	xmldomnode	*functionnode=node;

	// get the second expression node
	xmldomnode	*secondexpressionnode=
			node->getFirstTagChild(sqlparser::_parameters)->
				getFirstTagChild(sqlparser::_parameter)->
				getNextTagSibling(sqlparser::_parameter)->
				getFirstTagChild(sqlparser::_expression);
	if (secondexpressionnode->isNullNode()) {
		return true;
	}

	// get the interval qualifier node
	xmldomnode	*intervalqualifiernode=
		secondexpressionnode->
			getFirstTagChild(sqlparser::_interval_qualifier);
	if (intervalqualifiernode->isNullNode()) {
		return true;
	}

	// translate the interval qualifier to a string format...

	// get the start index
	const char	*startstr=intervalqualifiernode->
					getAttributeValue(
					sqlparser::_first_time_component);
	timeparts_t	start=TIMEPARTS_YEAR;
	while (start!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(startstr,timeparts[start])) {
		start=(timeparts_t)((uint16_t)start+1);
	}

	// get the end index
	const char	*endstr=intervalqualifiernode->
					getAttributeValue(
					sqlparser::_second_time_component);
	timeparts_t	end=TIMEPARTS_YEAR;
	while (end!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(endstr,timeparts[end])) {
		end=(timeparts_t)((uint16_t)end+1);
	}

	// validate start and end
	if (start==TIMEPARTS_NULL || end==TIMEPARTS_NULL || end<start) {
		return true;
	}

	// build up the format string
	stringbuffer	formatstring;
	formatstring.append("'");
	if (start<=TIMEPARTS_YEAR && end>=TIMEPARTS_YEAR) {
		formatstring.append("YYYY");
		if (end!=TIMEPARTS_YEAR) {
			formatstring.append("-");
		}
	}
	if (start<=TIMEPARTS_MONTH && end>=TIMEPARTS_MONTH) {
		formatstring.append("MM");
		if (end!=TIMEPARTS_MONTH) {
			formatstring.append("-");
		}
	}
	if (start<=TIMEPARTS_DAY && end>=TIMEPARTS_DAY) {
		formatstring.append("DD");
		if (end!=TIMEPARTS_DAY) {
			formatstring.append(" ");
		}
	}
	if (start<=TIMEPARTS_HOUR && end>=TIMEPARTS_HOUR) {
		formatstring.append("HH");
		if (end!=TIMEPARTS_HOUR) {
			formatstring.append(":");
		}
	}
	if (start<=TIMEPARTS_MINUTE && end>=TIMEPARTS_MINUTE) {
		formatstring.append("MI");
		if (end!=TIMEPARTS_MINUTE) {
			formatstring.append(":");
		}
	}
	if (start<=TIMEPARTS_SECOND && end>=TIMEPARTS_SECOND) {
		formatstring.append("SS");
		if (end!=TIMEPARTS_SECOND) {
			formatstring.append(":");
		}
	}
	if (start<=TIMEPARTS_FRACTION && end>=TIMEPARTS_FRACTION) {
		formatstring.append("FF");
		formatstring.append(intervalqualifiernode->
					getAttributeValue(sqlparser::_scale));
	}
	formatstring.append("'");

	// If we've gotten this far then we have an extend() function with
	// a second parameter expression of type interval_qualifier with
	// valid attributes.  Perform the translation...

	// translate extend() to to_char()
	functionnode->setAttributeValue(sqlparser::_value,"to_char");

	// delete the interval_qualifier node
	secondexpressionnode->deleteChild(intervalqualifiernode);

	// create a new string_literal node with the replacement format string
	sqlts->newNode(secondexpressionnode,
			sqlparser::_string_literal,
			formatstring.getString());

	// now wrap the whole thing in a to_date with the same format string
	xmldomnode	*todatenode=sqlts->newNode(functionnode->getParent(),
							sqlparser::_function,
							"to_date");
	xmldomnode	*todateparameters=sqlts->newNode(todatenode,
							sqlparser::_parameters);
	xmldomnode	*todateparameter1=sqlts->newNode(todateparameters,
							sqlparser::_parameter);
	xmldomnode	*todateexpression1=sqlts->newNode(todateparameter1,
							sqlparser::_expression);
	functionnode->getParent()->moveChild(functionnode,todateexpression1,0);
	xmldomnode	*todateparameter2=sqlts->newNode(todateparameters,
							sqlparser::_parameter);
	xmldomnode	*todateexpression2=sqlts->newNode(todateparameter2,
							sqlparser::_expression);
	sqlts->newNode(todateexpression2,
			sqlparser::_string_literal,
			formatstring.getString());
	return true;
}
