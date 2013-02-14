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

class informixtomssqlserverdate : public sqltranslation {
	public:
			informixtomssqlserverdate(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		bool	translateFunctions(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node,
						bool *found);
		bool	translateExtend(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node);
		void	evaluateIntervalQualifier(timeparts_t start,
						timeparts_t end,
						uint16_t *substringstart,
						uint16_t *substringend,
						const char **prepend,
						const char **append);
		void	tweakDateString(xmldomnode *functionnode,
						const char *prepend,
						const char *append);
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
		void	translateDateTimeString(const char *indtstring,
						stringbuffer *outdtstring,
						xmldomnode *iqnode);
		xmldomnode	*wrapSubstring(xmldomnode *functionnode,
						uint16_t start,
						uint16_t length);
		xmldomnode	*wrapConvert(xmldomnode *functionnode,
						const char *datatype,
						const char *formatstring);
};

informixtomssqlserverdate::informixtomssqlserverdate(sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool informixtomssqlserverdate::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {

	// The methods called inside of here rebuild the tree torrentially and
	// it's easy for a "current node" pointer to get lost entirely, so we
	// have to re-search the tree after each translation.  For now at least.
	bool	found=true;
	while (found) {
		if (!translateFunctions(sqlrcon,sqlrcur,
				querytree->getRootNode(),&found)) {
			return false;
		}
	}
	return true;
}

bool informixtomssqlserverdate::translateFunctions(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node,
						bool *found) {
	debugFunction();

	*found=false;

	// look for a function
	if (!charstring::compare(
			node->getName(),sqlparser::_function)) {

		// look for and translate extend, current, datetime
		// and interval functions
		const char	*value=
				node->getAttributeValue(sqlparser::_value);
		if (!charstring::compareIgnoringCase(value,"extend")) {
			*found=true;
			return translateExtend(sqlrcon,sqlrcur,node);
		} else if (!charstring::compareIgnoringCase(value,"current") ||
			!charstring::compareIgnoringCase(value,"call_dtime")) {
			*found=true;
			return translateCurrentDate(sqlrcon,sqlrcur,node);
		} else if (!charstring::compareIgnoringCase(value,"datetime")) {
			*found=true;
			return translateDateTime(sqlrcon,sqlrcur,node);
		} else if (!charstring::compareIgnoringCase(value,"interval")) {
			*found=true;
			return translateInterval(sqlrcon,sqlrcur,node);
		}
	}

	// convert child nodes...
	for (node=node->getFirstTagChild();
			!node->isNullNode(); node=node->getNextTagSibling()) {
		if (!translateFunctions(sqlrcon,sqlrcur,node,found)) {
			return false;
		}
	}
	return true;
}

bool informixtomssqlserverdate::translateExtend(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {
	debugFunction();

	// extend(date, interval_qualifier) ->
	// 	to_date(to_char(date, format_string), format_string)
	// ...

	// get the function node
	xmldomnode	*functionnode=node;

	// get the parameters node
	xmldomnode	*parametersnode=
			node->getFirstTagChild(sqlparser::_parameters);
	if (parametersnode->isNullNode()) {
		return true;
	}

	// get the first parameter node
	xmldomnode	*firstparameternode=
			parametersnode->getFirstTagChild(sqlparser::_parameter);
	if (firstparameternode->isNullNode()) {
		return true;
	}

	// get the first expression node
	xmldomnode	*firstexpressionnode=
				firstparameternode->
				getFirstTagChild(sqlparser::_expression);
	if (firstexpressionnode->isNullNode()) {
		return true;
	}

	// get the second expression node
	xmldomnode	*secondexpressionnode=
				firstparameternode->
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

	// get the interval qualifier start index
	const char	*startstr=iqnode->getAttributeValue(sqlparser::_from);
	timeparts_t	start=TIMEPARTS_YEAR;
	while (start!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(startstr,timeparts[start])) {
		start=(timeparts_t)((uint16_t)start+1);
	}

	// get the interval qualifier end index
	const char	*endstr=iqnode->getAttributeValue(sqlparser::_to);
	timeparts_t	end=TIMEPARTS_YEAR;
	while (end!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(endstr,timeparts[end])) {
		end=(timeparts_t)((uint16_t)end+1);
	}

	// validate start and end
	if (start==TIMEPARTS_NULL || end==TIMEPARTS_NULL || end<start) {
		return true;
	}

	// If we've gotten this far then we have an extend() function with
	// a second parameter expression of type interval_qualifier with
	// valid attributes.  Perform the translation...

	// translate extend() to convert()
	functionnode->setAttributeValue(sqlparser::_value,"convert");

	// insert a varchar parameter
	xmldomnode	*newparameternode=sqlts->newNodeBefore(
						parametersnode,
						firstparameternode,
						sqlparser::_parameter);
	xmldomnode	*newexpressionnode=sqlts->newNode(
						newparameternode,
						sqlparser::_expression);
	sqlts->newNode(newexpressionnode,sqlparser::_string_literal,"varchar");

	// delete the interval_qualifier node and replace it with a new
	// string_literal node with the replacement format string
	secondexpressionnode->deleteChild(iqnode);
	sqlts->newNode(secondexpressionnode,sqlparser::_string_literal,"21");

	// we have to truncate the date/time based on the
	// start and end index so wrap everything in a substring call
	uint16_t	substringstart=0;
	uint16_t	substringend=0;
	const char	*prepend=NULL;
	const char	*append=NULL;
	evaluateIntervalQualifier(start,end,
				&substringstart,
				&substringend,
				&prepend,&append);
	functionnode=wrapSubstring(functionnode,
				substringstart,substringend-substringstart);

	// wrap everything with a function to convert back to a datetime
	wrapConvert(functionnode,"datetime","21");

	// Handle some special cases.  SQL Server doesn't like dates that are
	// missing leading parts or trailing days or minutes, so go back and
	// tweak those here.
	// (don't move this above the wrapConvert() or it will cause problems)
	tweakDateString(functionnode,prepend,append);

	return true;
}

void informixtomssqlserverdate::evaluateIntervalQualifier(timeparts_t start,
						timeparts_t end,
						uint16_t *substringstart,
						uint16_t *substringend,
						const char **prepend,
						const char **append) {
	switch (start) {
		case TIMEPARTS_YEAR:
			*substringstart=0;
			break;
		case TIMEPARTS_MONTH:
			*substringstart=6;
			*prepend="'1900-'";
			break;
		case TIMEPARTS_DAY:
			*substringstart=9;
			*prepend="'1900-01-'";
			break;
		case TIMEPARTS_HOUR:
			*substringstart=12;
			break;
		case TIMEPARTS_MINUTE:
			*substringstart=15;
			*prepend="'00:'";
			break;
		case TIMEPARTS_SECOND:
			*substringstart=18;
			*prepend="'00:00:'";
			break;
		default:
			*substringstart=21;
			*prepend="'00:00:00.'";
			break;
	}
	switch (end) {
		case TIMEPARTS_YEAR:
			*substringend=5;
			break;
		case TIMEPARTS_MONTH:
			*substringend=8;
			*append="'-01'";
			break;
		case TIMEPARTS_DAY:
			*substringend=11;
			break;
		case TIMEPARTS_HOUR:
			*substringend=14;
			*append="':00'";
			break;
		case TIMEPARTS_MINUTE:
			*substringend=17;
			break;
		case TIMEPARTS_SECOND:
			*substringend=20;
			break;
		default:
			*substringend=24;
			break;
	}
}

void informixtomssqlserverdate::tweakDateString(xmldomnode *functionnode,
						const char *prepend,
						const char *append) {
	if (prepend) {
		xmldomnode	*prependliteral=
			sqlts->newNodeBefore(functionnode->getParent(),
						functionnode,
						sqlparser::_string_literal,
						prepend);
		sqlts->newNodeAfter(functionnode->getParent(),
						prependliteral,
						sqlparser::_plus);
	}
	if (append) {
		xmldomnode	*plus=
			sqlts->newNodeAfter(functionnode->getParent(),
						functionnode,
						sqlparser::_plus);
		sqlts->newNodeAfter(functionnode->getParent(),
						plus,
						sqlparser::_string_literal,
						append);
	}
}

bool informixtomssqlserverdate::translateCurrentDate(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *node) {

	// "function" -> sysdatetime()
	// or
	// "function" interval_qualifier -> sysdatetime()
	// ...

	debugFunction();

	// translate the current-date function to sysdatetime
	node->setAttributeValue(sqlparser::_value,"sysdatetime");

	// add an empty parameter set
	sqlts->newNode(node,sqlparser::_parameters);

	// get the interval qualifier node, if there is one...
	xmldomnode	*iqnode=node->getNextTagSibling();
	if (iqnode->isNullNode() ||
		charstring::compare(iqnode->getName(),
					sqlparser::_interval_qualifier)) {

		// if there was no interval qualifier then just wrap
		// everything with a function to convert to a datetime
		// (as opposed to datetime2 which no math can be done on)
		wrapConvert(node,"datetime","21");
		return true;
	}

	// get the interval qualifier start index
	const char	*startstr=iqnode->getAttributeValue(sqlparser::_from);
	timeparts_t	start=TIMEPARTS_YEAR;
	while (start!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(startstr,timeparts[start])) {
		start=(timeparts_t)((uint16_t)start+1);
	}

	// get the interval qualifier end index
	const char	*endstr=iqnode->getAttributeValue(sqlparser::_to);
	timeparts_t	end=TIMEPARTS_YEAR;
	while (end!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(endstr,timeparts[end])) {
		end=(timeparts_t)((uint16_t)end+1);
	}

	// validate start and end
	if (start==TIMEPARTS_NULL || end==TIMEPARTS_NULL || end<start) {
		return true;
	}

	// If we've gotten this far then we have a function with a
	// second parameter expression of type interval_qualifier with
	// valid attributes.  Perform the translation...

	// delete the interval_qualifier node
	node->getParent()->deleteChild(iqnode);

	// wrap everything in a convert function
	xmldomnode	*convertnode=wrapConvert(node,"varchar","21");

	// we have to truncate the date/time based on the
	// start and end index so wrap everything in a substring call
	uint16_t	substringstart=0;
	uint16_t	substringend=0;
	const char	*prepend=NULL;
	const char	*append=NULL;
	evaluateIntervalQualifier(start,end,
				&substringstart,
				&substringend,
				&prepend,&append);
	xmldomnode	*substringnode=wrapSubstring(convertnode,
							substringstart,
							substringend-
							substringstart);

	// wrap everything with a function to convert back to a datetime
	wrapConvert(substringnode,"datetime","21");

	// Handle some special cases.  SQL Server doesn't like dates that are
	// missing leading parts or trailing days or minutes, so go back and
	// tweak those here.
	// (don't move this above the wrapConvert() or it will cause problems)
	tweakDateString(substringnode,prepend,append);

	return true;
}

bool informixtomssqlserverdate::translateDateTime(sqlrconnection_svr *sqlrcon,
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
	// (we'll use sql server's format "21" (yyyy-mm-dd hh:mm:ss.fff) to
	// parse the datetime but we need to reformat it to match that as
	// closely as possible first)
	stringbuffer	datetimestring;
	translateDateTimeString(stringliteralnode->
				getAttributeValue(sqlparser::_value),
				&datetimestring,iqnode);
	stringliteralnode->setAttributeValue(sqlparser::_value,
						datetimestring.getString());

	// delete the interval_qualifier node
	functionnode->getParent()->deleteChild(iqnode);

	// add a format string parameter
	newparameternode=sqlts->newNodeAfter(parametersnode,
						firstparameternode,
						sqlparser::_parameter);
	newexpressionnode=sqlts->newNode(newparameternode,
						sqlparser::_expression);
	sqlts->newNode(newexpressionnode,sqlparser::_string_literal,"21");

	return true;
}

bool informixtomssqlserverdate::translateInterval(sqlrconnection_svr *sqlrcon,
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

void informixtomssqlserverdate::compressIntervalQualifier(xmldomnode *iqnode) {
	const char	*from=iqnode->getAttributeValue("from");
	const char	*to=iqnode->getAttributeValue("to");
	if (!charstring::compare(from,to)) {
		iqnode->deleteAttribute(sqlparser::_to);
		iqnode->deleteAttribute(sqlparser::_to_precision);
		iqnode->deleteAttribute(sqlparser::_to_scale);
	}
}

void informixtomssqlserverdate::translateDateTimeString(
					const char *indtstring,
					stringbuffer *outdtstring,
					xmldomnode *iqnode) {

	// get the interval qualifier start index
	const char	*startstr=iqnode->getAttributeValue(sqlparser::_from);
	timeparts_t	start=TIMEPARTS_YEAR;
	while (start!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(startstr,timeparts[start])) {
		start=(timeparts_t)((uint16_t)start+1);
	}

	// get the interval qualifier end index
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

	// start building the string, it needs to be quoted
	outdtstring->append('\'');

	// if we started with something other than the year or day then
	// prepend the rest from the current date/time
	datetime	dt;
	dt.getSystemDateAndTime();
	switch (start) {
		case TIMEPARTS_MONTH:
			outdtstring->append(dt.getYear())->append('-');
			break;
		case TIMEPARTS_DAY:
			outdtstring->append(dt.getYear())->append('-');
			outdtstring->append(dt.getMonth())->append('-');
			break;
		case TIMEPARTS_MINUTE:
			outdtstring->append(dt.getHour())->append(':');
			break;
		case TIMEPARTS_SECOND:
			outdtstring->append(dt.getHour())->append(':');
			outdtstring->append(dt.getMinutes());
			outdtstring->append(':');
			break;
		case TIMEPARTS_FRACTION:
			outdtstring->append(dt.getHour())->append(':');
			outdtstring->append(dt.getMinutes());
			outdtstring->append(':');
			outdtstring->append(dt.getSeconds());
			outdtstring->append('.');
			break;
		default:
			break;
	}

	// append the string that was passed in
	outdtstring->append(indtstring);

	// if we ended with anything short of a full date then append the rest
	switch (end) {
		case TIMEPARTS_YEAR:
			outdtstring->append("-01-01 00:00:00.000");
			break;
		case TIMEPARTS_MONTH:
			outdtstring->append("-01 00:00:00.000");
			break;
		case TIMEPARTS_DAY:
			outdtstring->append(" 00:00:00.000");
			break;
		case TIMEPARTS_HOUR:
			outdtstring->append(":00:00.000");
			break;
		case TIMEPARTS_MINUTE:
			outdtstring->append(":00.000");
			break;
		case TIMEPARTS_SECOND:
			outdtstring->append(".000");
			break;
		default:
			break;
	}

	outdtstring->append('\'');
}

xmldomnode *informixtomssqlserverdate::wrapSubstring(xmldomnode *functionnode,
							uint16_t start,
							uint16_t length) {

	xmldomnode	*substringnode=sqlts->newNodeBefore(
						functionnode->getParent(),
						functionnode,
						sqlparser::_function,
						"substring");
	xmldomnode	*substringparameters=sqlts->newNode(
						substringnode,
						sqlparser::_parameters);
	xmldomnode	*substringparameter1=sqlts->newNode(
						substringparameters,
						sqlparser::_parameter);
	xmldomnode	*substringexpression1=sqlts->newNode(
						substringparameter1,
						sqlparser::_expression);
	functionnode->getParent()->moveChild(functionnode,
						substringexpression1,0);
	xmldomnode	*substringparameter2=sqlts->newNode(
						substringparameters,
						sqlparser::_parameter);
	xmldomnode	*substringexpression2=sqlts->newNode(
						substringparameter2,
						sqlparser::_expression);
	char	*startstr=charstring::parseNumber(start);
	sqlts->newNode(substringexpression2,
				sqlparser::_string_literal,startstr);
	delete[] startstr;
	xmldomnode	*substringparameter3=sqlts->newNode(
						substringparameters,
						sqlparser::_parameter);
	xmldomnode	*substringexpression3=sqlts->newNode(
						substringparameter3,
						sqlparser::_expression);
	char	*lengthstr=charstring::parseNumber(length);
	sqlts->newNode(substringexpression3,
				sqlparser::_string_literal,lengthstr);
	delete[] lengthstr;
	return substringnode;
}

xmldomnode *informixtomssqlserverdate::wrapConvert(xmldomnode *functionnode,
						const char *datatype,
						const char *formatstring) {

	xmldomnode	*convertnode=sqlts->newNodeBefore(
						functionnode->getParent(),
						functionnode,
						sqlparser::_function,
						"convert");
	xmldomnode	*convertparameters=sqlts->newNode(
						convertnode,
						sqlparser::_parameters);
	xmldomnode	*convertparameter1=sqlts->newNode(
						convertparameters,
						sqlparser::_parameter);
	xmldomnode	*convertexpression1=sqlts->newNode(
						convertparameter1,
						sqlparser::_expression);
	sqlts->newNode(convertexpression1,
				sqlparser::_string_literal,datatype);
	xmldomnode	*convertparameter2=sqlts->newNode(
						convertparameters,
						sqlparser::_parameter);
	xmldomnode	*convertexpression2=sqlts->newNode(
						convertparameter2,
						sqlparser::_expression);
	functionnode->getParent()->moveChild(functionnode,convertexpression2,0);
	xmldomnode	*convertparameter3=sqlts->newNode(
						convertparameters,
						sqlparser::_parameter);
	xmldomnode	*convertexpression3=sqlts->newNode(
						convertparameter3,
						sqlparser::_expression);
	sqlts->newNode(convertexpression3,
				sqlparser::_string_literal,formatstring);
	return convertnode;
}

extern "C" {
	sqltranslation	*new_informixtomssqlserverdate(
					sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new informixtomssqlserverdate(sqlts,parameters);
	}
}
