// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltranslations/informixtooracledate.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	sqltranslation	*new_informixtooracledate(
					sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new informixtooracledate(sqlts,parameters);
	}
}

informixtooracledate::informixtooracledate(sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool informixtooracledate::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {

	return translateFunctions(sqlrcon,sqlrcur,querytree->getRootNode());
}

bool informixtooracledate::translateFunctions(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {
	debugFunction();

	// look for a function
	if (!charstring::compare(
			node->getName(),sqlparser::_function)) {

		// look for and translate extend, current, datetime
		// and interval functions
		if (!charstring::compareIgnoringCase(
				node->getAttributeValue(sqlparser::_value),
				"extend")) {
			if (!translateExtend(sqlrcon,sqlrcur,node)) {
				return false;
			}
		} else if (!charstring::compareIgnoringCase(
				node->getAttributeValue(sqlparser::_value),
				"current") ||
			!charstring::compareIgnoringCase(
				node->getAttributeValue(sqlparser::_value),
				"call_dtime")) {
			if (!translateCurrentDate(sqlrcon,sqlrcur,node)) {
				return false;
			}
		} else if (!charstring::compareIgnoringCase(
			node->getAttributeValue(sqlparser::_value),
			"datetime")) {
			if (!translateDateTime(sqlrcon,sqlrcur,node)) {
				return false;
			}
		} else if (!charstring::compareIgnoringCase(
			node->getAttributeValue(sqlparser::_value),
			"interval")) {
			if (!translateInterval(sqlrcon,sqlrcur,node)) {
				return false;
			}
		}
	}

	// convert child nodes...
	for (node=node->getFirstTagChild();
			!node->isNullNode(); node=node->getNextTagSibling()) {
		if (!translateFunctions(sqlrcon,sqlrcur,node)) {
			return false;
		}
	}
	return true;
}

bool informixtooracledate::translateExtend(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {
	debugFunction();

	// extend(date, interval_qualifier) ->
	// 	to_date(to_char(date, format_string), format_string)
	// ...

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
				secondexpressionnode->getFirstTagChild(
						sqlparser::_interval_qualifier);
	if (intervalqualifiernode->isNullNode()) {
		return true;
	}

	// translate the interval qualifier to a string format...
	stringbuffer	formatstring;
	translateIntervalQualifier(&formatstring,intervalqualifiernode);

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
	wrapToDate(functionnode,formatstring.getString());

	return true;
}

bool informixtooracledate::translateCurrentDate(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {

	// "function" -> systimestamp
	// or
	// "function" interval_qualifier ->
	// 	to_date(to_char(systimestamp, format_string), format_string)
	// ...

	debugFunction();

	// get the function node
	xmldomnode	*functionnode=node;

	// translate the current-date function to systimestamp
	functionnode->setAttributeValue(sqlparser::_value,"systimestamp");

	// get the interval qualifier node, if there is one...
	xmldomnode	*intervalqualifiernode=
				functionnode->getNextTagSibling(
					sqlparser::_interval_qualifier);
	if (intervalqualifiernode->isNullNode()) {
		return true;
	}

	// translate the interval qualifier to a string format...
	stringbuffer	formatstring;
	translateIntervalQualifier(&formatstring,intervalqualifiernode);

	// delete the interval_qualifier node
	functionnode->getParent()->deleteChild(intervalqualifiernode);

	// wrap the whole thing in a
	// to_date(to_char(..., formatstring), formatstring)
	wrapBoth(functionnode,formatstring.getString());

	return true;
}

bool informixtooracledate::translateDateTime(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {

	// datetime(...) interval_qualifier -> to_date(..., format_string)
	// ...

	debugFunction();

	// get the function node
	xmldomnode	*functionnode=node;

	// get the parameters node, if there is one...
	xmldomnode	*parametersnode=
				functionnode->getFirstTagChild(
						sqlparser::_parameters);
	if (parametersnode->isNullNode()) {
		return true;
	}

	// get the first parameter node, if there is one...
	xmldomnode	*firstparameternode=
				parametersnode->getFirstTagChild(
						sqlparser::_parameter);
	if (firstparameternode->isNullNode()) {
		return true;
	}

	// get the first parameter string literal node, if there is one...
	xmldomnode	*stringliteralnode=
				firstparameternode->getFirstTagChild(
						sqlparser::_string_literal);
	if (stringliteralnode->isNullNode()) {
		return true;
	}

	// get the interval qualifier node, if there is one...
	xmldomnode	*intervalqualifiernode=
				functionnode->getNextTagSibling(
					sqlparser::_interval_qualifier);
	if (intervalqualifiernode->isNullNode()) {
		return true;
	}

	// translate datetime to to_date
	functionnode->setAttributeValue(sqlparser::_value,"to_date");

	// quote the date/time parameter
	stringbuffer	newdatetimeparam;
	newdatetimeparam.append('\'');
	newdatetimeparam.append(
		stringliteralnode->getAttributeValue(sqlparser::_value));
	newdatetimeparam.append('\'');
	stringliteralnode->setAttributeValue(sqlparser::_value,
						newdatetimeparam.getString());

	// translate the interval qualifier to a string format...
	stringbuffer	formatstring;
	translateIntervalQualifier(&formatstring,intervalqualifiernode);

	// delete the interval_qualifier node
	functionnode->getParent()->deleteChild(intervalqualifiernode);

	// add a format string parameter
	xmldomnode	*newparameternode=sqlts->newNodeAfter(
						parametersnode,
						firstparameternode,
						sqlparser::_parameter);
	xmldomnode	*newexpressionnode=sqlts->newNode(
						newparameternode,
						sqlparser::_expression);
	sqlts->newNode(newexpressionnode,sqlparser::_string_literal,
						formatstring.getString());

	return true;
}

bool informixtooracledate::translateInterval(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {

	// interval(...) interval_qualifier -> interval '...' interval_qualifier
	debugFunction();

	// get the function node
	xmldomnode	*functionnode=node;

	// get the interval value
	const char	*interval=
				node->getFirstTagChild(sqlparser::_parameters)->
				getFirstTagChild(sqlparser::_parameter)->
				getFirstTagChild(sqlparser::_string_literal)->
				getAttributeValue(sqlparser::_value);

	// bail if there was no single-number interval value
	if (!charstring::length(interval)) {
		return true;
	}

	// put quotes around the interval value
	stringbuffer	quotedinterval;
	quotedinterval.append('\'')->append(interval)->append('\'');

	// create a string literal containing the expression,
	// after the function, before the interval qualifier
	sqlts->newNodeAfter(functionnode->getParent(),functionnode,
				sqlparser::_string_literal,
				quotedinterval.getString());

	// delete the parameters
	functionnode->deleteChild(
			functionnode->getFirstTagChild(sqlparser::_parameters));

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

void informixtooracledate::translateIntervalQualifier(
					stringbuffer *formatstring,
					xmldomnode *intervalqualifiernode) {

	// get the start index
	const char	*startstr=intervalqualifiernode->
					getAttributeValue(sqlparser::_from);
	timeparts_t	start=TIMEPARTS_YEAR;
	while (start!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(startstr,timeparts[start])) {
		start=(timeparts_t)((uint16_t)start+1);
	}

	// get the end index
	const char	*endstr=intervalqualifiernode->
					getAttributeValue(sqlparser::_to);
	timeparts_t	end=TIMEPARTS_YEAR;
	while (end!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(endstr,timeparts[end])) {
		end=(timeparts_t)((uint16_t)end+1);
	}

	// validate start and end
	if (start==TIMEPARTS_NULL || end==TIMEPARTS_NULL || end<start) {
		return;
	}

	// build up the format string
	formatstring->append("'");
	if (start<=TIMEPARTS_YEAR && end>=TIMEPARTS_YEAR) {
		formatstring->append("YYYY");
		if (end!=TIMEPARTS_YEAR) {
			formatstring->append("-");
		}
	}
	if (start<=TIMEPARTS_MONTH && end>=TIMEPARTS_MONTH) {
		formatstring->append("MM");
		if (end!=TIMEPARTS_MONTH) {
			formatstring->append("-");
		}
	}
	if (start<=TIMEPARTS_DAY && end>=TIMEPARTS_DAY) {
		formatstring->append("DD");
		if (end!=TIMEPARTS_DAY) {
			formatstring->append(" ");
		}
	}
	if (start<=TIMEPARTS_HOUR && end>=TIMEPARTS_HOUR) {
		formatstring->append("HH");
		if (end!=TIMEPARTS_HOUR) {
			formatstring->append(":");
		}
	}
	if (start<=TIMEPARTS_MINUTE && end>=TIMEPARTS_MINUTE) {
		formatstring->append("MI");
		if (end!=TIMEPARTS_MINUTE) {
			formatstring->append(":");
		}
	}
	if (start<=TIMEPARTS_SECOND && end>=TIMEPARTS_SECOND) {
		formatstring->append("SS");
		if (end!=TIMEPARTS_SECOND) {
			formatstring->append(".");
		}
	}
	if (start<=TIMEPARTS_FRACTION && end>=TIMEPARTS_FRACTION) {
		formatstring->append("FF");
		formatstring->append(intervalqualifiernode->
					getAttributeValue(sqlparser::_scale));
	}
	formatstring->append("'");
}

xmldomnode *informixtooracledate::wrapBoth(xmldomnode *functionnode,
						const char *formatstring) {
	xmldomnode	*tocharnode=wrapToChar(functionnode,formatstring);
	return wrapToDate(tocharnode,formatstring);
}

xmldomnode *informixtooracledate::wrapToChar(xmldomnode *functionnode,
						const char *formatstring) {
	return wrap(functionnode,"to_char",formatstring);
}

xmldomnode *informixtooracledate::wrapToDate(xmldomnode *functionnode,
						const char *formatstring) {
	return wrap(functionnode,"to_date",formatstring);
}

xmldomnode *informixtooracledate::wrap(xmldomnode *functionnode,
					const char *function,
					const char *formatstring) {

	xmldomnode	*toxxxnode=sqlts->newNode(functionnode->getParent(),
							sqlparser::_function,
							function);
	xmldomnode	*toxxxparameters=sqlts->newNode(toxxxnode,
							sqlparser::_parameters);
	xmldomnode	*toxxxparameter1=sqlts->newNode(toxxxparameters,
							sqlparser::_parameter);
	xmldomnode	*toxxxexpression1=sqlts->newNode(toxxxparameter1,
							sqlparser::_expression);
	functionnode->getParent()->moveChild(functionnode,toxxxexpression1,0);
	xmldomnode	*toxxxparameter2=sqlts->newNode(toxxxparameters,
							sqlparser::_parameter);
	xmldomnode	*toxxxexpression2=sqlts->newNode(toxxxparameter2,
							sqlparser::_expression);
	sqlts->newNode(toxxxexpression2,
				sqlparser::_string_literal,
				formatstring);
	return toxxxnode;
}
