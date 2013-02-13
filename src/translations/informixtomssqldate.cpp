// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslation.h>
#include <rudiments/datetime.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class informixtomssqldate : public sqltranslation {
	public:
			informixtomssqldate(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		bool	translateFunctions(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *node);
		bool	translateExtend(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *node);
		bool	translateCurrentDate(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *node);
		bool	translateDateTime(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *node);
		bool	translateInterval(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *node);
		void	compressIntervalQualifier(xmldomnode *iqnode);
		void	translateDateTimeString(
					const char *indtstring,
					stringbuffer *outdtstring,
					xmldomnode *iqnode);
		void	translateIntervalQualifier(
					stringbuffer *formatstring,
					xmldomnode *iqnode,
					bool *containsfraction);

		xmldomnode	*wrapBoth(xmldomnode *functionnode,
					const char *formatstring,
					bool containsfraction);
		xmldomnode	*wrapToChar(xmldomnode *functionnode,
					const char *formatstring);
		xmldomnode	*wrapToDate(xmldomnode *functionnode,
					const char *formatstring,
					bool containsfraction);
		xmldomnode	*wrap(xmldomnode *functionnode,
					const char *function,
					const char *formatstring);
};

informixtomssqldate::informixtomssqldate(sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool informixtomssqldate::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	return translateFunctions(sqlrcon,sqlrcur,querytree->getRootNode());
}

bool informixtomssqldate::translateFunctions(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {
	debugFunction();

	// look for a function
	if (!charstring::compare(
			node->getName(),sqlparser::_function)) {

		// look for and translate extend, current, datetime
		// and interval functions
		const char	*value=
				node->getAttributeValue(sqlparser::_value);
		if (!charstring::compareIgnoringCase(value,"extend")) {
			if (!translateExtend(sqlrcon,sqlrcur,node)) {
				return false;
			}
		} else if (!charstring::compareIgnoringCase(value,"current") ||
			!charstring::compareIgnoringCase(value,"call_dtime")) {
			if (!translateCurrentDate(sqlrcon,sqlrcur,node)) {
				return false;
			}
		} else if (!charstring::compareIgnoringCase(value,"datetime")) {
			if (!translateDateTime(sqlrcon,sqlrcur,node)) {
				return false;
			}
		} else if (!charstring::compareIgnoringCase(value,"interval")) {
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

bool informixtomssqldate::translateExtend(sqlrconnection_svr *sqlrcon,
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
	xmldomnode	*iqnode=secondexpressionnode->getFirstTagChild(
						sqlparser::_interval_qualifier);
	if (iqnode->isNullNode()) {
		return true;
	}

	// translate the interval qualifier to a string format...
	stringbuffer	formatstring;
	bool		containsfraction;
	translateIntervalQualifier(&formatstring,iqnode,&containsfraction);

	// If we've gotten this far then we have an extend() function with
	// a second parameter expression of type interval_qualifier with
	// valid attributes.  Perform the translation...

	// translate extend() to to_char()
	functionnode->setAttributeValue(sqlparser::_value,"to_char");

	// delete the interval_qualifier node
	secondexpressionnode->deleteChild(iqnode);

	// create a new string_literal node with the replacement format string
	sqlts->newNode(secondexpressionnode,
			sqlparser::_string_literal,
			formatstring.getString());

	// now wrap the whole thing in a to_date with the same format string
	wrapToDate(functionnode,formatstring.getString(),containsfraction);

	return true;
}

bool informixtomssqldate::translateCurrentDate(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {

	// "function" -> sysdatetime()
	// or
	// "function" interval_qualifier ->
	// 	to_date(to_char(sysdatetime, format_string), format_string)
	// ...

	debugFunction();

	// translate the current-date function to sysdatetime
	node->setAttributeValue(sqlparser::_value,"sysdatetime");

	// add an empty parameter set
	node->appendChild(new xmldomnode(node->getTree(),
					node->getNullNode(),
					TAG_XMLDOMNODETYPE,
					sqlparser::_parameters,NULL));

// FIXME: handle interval qualifiers...
	// get the interval qualifier node, if there is one...
	xmldomnode	*nextnode=node->getNextTagSibling();
	if (nextnode->isNullNode() ||
		charstring::compare(nextnode->getName(),
					sqlparser::_interval_qualifier)) {
		return true;
	}

	// translate the interval qualifier to a format string...
	stringbuffer	formatstring;
	bool		containsfraction;
	translateIntervalQualifier(&formatstring,nextnode,&containsfraction);

	// delete the interval_qualifier node
	node->getParent()->deleteChild(nextnode);

	// wrap the whole thing in a
	// to_date(to_char(..., formatstring), formatstring)
	wrapBoth(node,formatstring.getString(),containsfraction);

	return true;
}

bool informixtomssqldate::translateDateTime(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {

	// datetime(...) interval_qualifier -> convert(datetime, ..., format)
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
	xmldomnode	*iqnode=functionnode->getNextTagSibling(
					sqlparser::_interval_qualifier);
	if (iqnode->isNullNode()) {
		return true;
	}

	// translate datetime function to convert
	functionnode->setAttributeValue(sqlparser::_value,"convert");

	// insert a datetime parameter
	xmldomnode	*newparameternode=sqlts->newNodeBefore(
						parametersnode,
						firstparameternode,
						sqlparser::_parameter);
	xmldomnode	*newexpressionnode=sqlts->newNode(
						newparameternode,
						sqlparser::_expression);
	sqlts->newNode(newexpressionnode,sqlparser::_string_literal,"datetime");

	// update the date/time string
	stringbuffer	datetimestring;
	translateDateTimeString(stringliteralnode->
				getAttributeValue(sqlparser::_value),
				&datetimestring,iqnode);
	stringliteralnode->setAttributeValue(sqlparser::_value,
						datetimestring.getString());

	// translate the interval qualifier to a string format...
	stringbuffer	formatstring;
	bool		containsfraction;
	translateIntervalQualifier(&formatstring,iqnode,&containsfraction);

	// delete the interval_qualifier node
	functionnode->getParent()->deleteChild(iqnode);

	// add a format string parameter
	newparameternode=sqlts->newNodeAfter(parametersnode,
						firstparameternode,
						sqlparser::_parameter);
	newexpressionnode=sqlts->newNode(newparameternode,
						sqlparser::_expression);
	sqlts->newNode(newexpressionnode,sqlparser::_string_literal,
						formatstring.getString());

	return true;
}

bool informixtomssqldate::translateInterval(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {

	// interval(...) interval_qualifier -> interval '...' interval_qualifier
	debugFunction();

	// get the interval value
	const char	*interval=
				node->getFirstTagChild(sqlparser::_parameters)->
				getFirstTagChild(sqlparser::_parameter)->
				getFirstTagChild(sqlparser::_string_literal)->
				getAttributeValue(sqlparser::_value);
	if (!charstring::length(interval)) {
		return true;
	}

	// get the next node, it could be an interval qualifier and since
	// we'll be modifying the current node we need to get it now
	xmldomnode	*iqnode=node->getNextTagSibling();

	// put quotes around the interval value
	stringbuffer	quotedinterval;
	quotedinterval.append('\'')->append(interval)->append('\'');

	// create a string literal containing the expression,
	// after the function, before the interval qualifier
	sqlts->newNodeAfter(node->getParent(),node,
				sqlparser::_string_literal,
				quotedinterval.getString());

	// delete the parameters
	node->deleteChild(node->getFirstTagChild(sqlparser::_parameters));

	// compress the interval qualifier, if there was one
	if (!charstring::compare(iqnode->getName(),
					sqlparser::_interval_qualifier)) {
		compressIntervalQualifier(iqnode);
	}

	return true;
}

void informixtomssqldate::compressIntervalQualifier(xmldomnode *iqnode) {
	const char	*from=iqnode->getAttributeValue("from");
	const char	*to=iqnode->getAttributeValue("to");
	if (!charstring::compare(from,to)) {
		iqnode->deleteAttribute(sqlparser::_to);
		iqnode->deleteAttribute(sqlparser::_to_precision);
		iqnode->deleteAttribute(sqlparser::_to_scale);
	}
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

void informixtomssqldate::translateDateTimeString(
					const char *indtstring,
					stringbuffer *outdtstring,
					xmldomnode *iqnode) {

	// get the start index
	const char	*startstr=iqnode->getAttributeValue(sqlparser::_from);
	timeparts_t	start=TIMEPARTS_YEAR;
	while (start!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(startstr,timeparts[start])) {
		start=(timeparts_t)((uint16_t)start+1);
	}

	// get the end index
	const char	*endstr=iqnode->getAttributeValue(sqlparser::_to);
	timeparts_t	end=TIMEPARTS_YEAR;
	while (end!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(endstr,timeparts[end])) {
		end=(timeparts_t)((uint16_t)end+1);
	}

	// validate start and end
	if (start==TIMEPARTS_NULL || end==TIMEPARTS_NULL || end<start) {
		return;
	}

	// get the current date/time, we might need it
	datetime	dt;
	dt.getSystemDateAndTime();

	// start building the string, it needs to be quoted
	outdtstring->append('\'');

	// if we started with the month or day then
	// prepend the current year or month
	if (start==TIMEPARTS_MONTH) {
		outdtstring->append(dt.getYear())->append('-');
	} else if (start==TIMEPARTS_DAY) {
		outdtstring->append(dt.getYear())->append('-');
		outdtstring->append(dt.getMonth())->append('-');
	} else if (start==TIMEPARTS_MINUTE) {
		outdtstring->append(dt.getHour())->append(':');
	} else if (start==TIMEPARTS_SECOND) {
		outdtstring->append(dt.getHour())->append(':');
		outdtstring->append(dt.getMinutes())->append(':');
	} else if (start==TIMEPARTS_FRACTION) {
		outdtstring->append(dt.getHour())->append(':');
		outdtstring->append(dt.getMinutes())->append(':');
		outdtstring->append(dt.getSeconds())->append('.');
	}

	// append the string that was passed in
	outdtstring->append(indtstring);

	// figure out what kind of date format we've got
	// FIXME: presumably it starts with either a year or hour, but that's
	// not guaranteed.  I'm not sure what to do in other cases yet.
	const char	*firstdash=NULL;
	const char	*seconddash=NULL;
	const char	*space=NULL;
	const char	*firstcolon=NULL;
	const char	*secondcolon=NULL;
	const char	*dot=NULL;
	firstdash=charstring::findFirst(indtstring,'-');
	if (firstdash) {
		seconddash=charstring::findFirst(firstdash+1,'-');
	}
	if (seconddash) {
		space=charstring::findFirst(seconddash+1,' ');
	}
	if (space) {
		firstcolon=charstring::findFirst(space+1,':');
	} else {
		firstcolon=charstring::findFirst(indtstring,':');
	}
	if (firstcolon) {
		secondcolon=charstring::findFirst(firstcolon+1,':');
	}
	if (secondcolon) {
		dot=charstring::findFirst(secondcolon+1,'.');
	}

	// There are a few problematic cases
	// that need to be extended.
	// YYYY-MM -> YYYY-MM-01
	// YYYY-MM-DD HH -> YYYY-MM-DD HH:00
	// HH -> HH:00
	if (start==TIMEPARTS_YEAR) {
		if (firstdash && !seconddash) {
			// handle: YYYY-MM -> YYYY-MM-01
			outdtstring->append("-01");
		} else if (firstdash && space && !firstcolon) {
			// handle: YYYY-MM-DD HH -> YYYY-MM-DD HH:00
			outdtstring->append(":00");
		}
	} else if (start==TIMEPARTS_HOUR) {
		if (!firstcolon) {
			// handle: HH -> HH:00
			outdtstring->append(":00");
		}
	}

	outdtstring->append('\'');
}

void informixtomssqldate::translateIntervalQualifier(
					stringbuffer *formatstring,
					xmldomnode *iqnode,
					bool *containsfraction) {

	*containsfraction=false;

	// get the start index
	const char	*startstr=iqnode->getAttributeValue(sqlparser::_from);
	timeparts_t	start=TIMEPARTS_YEAR;
	while (start!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(startstr,timeparts[start])) {
		start=(timeparts_t)((uint16_t)start+1);
	}

	// get the end index
	const char	*endstr=iqnode->getAttributeValue(sqlparser::_to);
	timeparts_t	end=TIMEPARTS_YEAR;
	while (end!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(endstr,timeparts[end])) {
		end=(timeparts_t)((uint16_t)end+1);
	}

	// validate start and end
	if (start==TIMEPARTS_NULL || end==TIMEPARTS_NULL || end<start) {
		return;
	}

	// anything starting with an hour (or smaller) uses format 14
	// anything else uses format 21 or 20
	if (start>=TIMEPARTS_HOUR) {
		formatstring->append("14");
	} else {
		formatstring->append((end==TIMEPARTS_FRACTION)?"21":"20");
	}
}

xmldomnode *informixtomssqldate::wrapBoth(xmldomnode *functionnode,
						const char *formatstring,
						bool containsfraction) {
	xmldomnode	*tocharnode=wrapToChar(functionnode,formatstring);
	return wrapToDate(tocharnode,formatstring,containsfraction);
}

xmldomnode *informixtomssqldate::wrapToChar(xmldomnode *functionnode,
						const char *formatstring) {
	return wrap(functionnode,"to_char",formatstring);
}

xmldomnode *informixtomssqldate::wrapToDate(xmldomnode *functionnode,
						const char *formatstring,
						bool containsfraction) {
	return wrap(functionnode,
			(containsfraction)?"to_timestamp":"to_date",
			formatstring);
}

xmldomnode *informixtomssqldate::wrap(xmldomnode *functionnode,
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

extern "C" {
	sqltranslation	*new_informixtomssqldate(
					sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new informixtomssqldate(sqlts,parameters);
	}
}
