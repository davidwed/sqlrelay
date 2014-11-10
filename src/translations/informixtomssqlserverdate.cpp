// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlreparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <rudiments/datetime.h>
#include <debugprint.h>

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

class informixtomssqlserverdate : public sqlrtranslation {
	public:
			informixtomssqlserverdate(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
	private:
		bool	translateFunctions(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldomnode *node,
						bool *found);
		bool	translateExtend(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
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
		bool	translateCurrentDate(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldomnode *node);
		bool	translateDateTime(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldomnode *node);
		bool	translateInterval(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldomnode *node);
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

informixtomssqlserverdate::informixtomssqlserverdate(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool informixtomssqlserverdate::usesTree() {
	return true;
}

bool informixtomssqlserverdate::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
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

bool informixtomssqlserverdate::translateFunctions(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldomnode *node,
						bool *found) {
	debugFunction();

	*found=false;

	// look for a function
	if (!charstring::compare(node->getName(),sqlreparser::_function)) {

		// look for and translate extend, current, datetime
		// and interval functions
		const char	*value=
				node->getAttributeValue(sqlreparser::_value);
		if (!charstring::compareIgnoringCase(value,"extend")) {
			*found=true;
			return translateExtend(sqlrcon,sqlrcur,node);
		} else if (!charstring::compareIgnoringCase(value,"sysdate") ||
			!charstring::compareIgnoringCase(value,
							"systimestamp") ||
			!charstring::compareIgnoringCase(value,
							"current_date") ||
			!charstring::compareIgnoringCase(value,"current") ||
			!charstring::compareIgnoringCase(value,"call_dtime") ||
			!charstring::compareIgnoringCase(value,"today")) {
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

bool informixtomssqlserverdate::translateExtend(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldomnode *node) {
	debugFunction();

	// extend(date, interval_qualifier) ->
	// 	to_date(to_char(date, format_string), format_string)
	// ...

	// get the function node
	xmldomnode	*functionnode=node;

	// get the parameters node
	xmldomnode	*parametersnode=
			node->getFirstTagChild(sqlreparser::_parameters);
	if (parametersnode->isNullNode()) {
		return true;
	}

	// get the first parameter node
	xmldomnode	*firstparameternode=
			parametersnode->getFirstTagChild(sqlreparser::_parameter);
	if (firstparameternode->isNullNode()) {
		return true;
	}

	// get the first expression node
	xmldomnode	*firstexpressionnode=
				firstparameternode->
				getFirstTagChild(sqlreparser::_expression);
	if (firstexpressionnode->isNullNode()) {
		return true;
	}

	// get the second expression node
	xmldomnode	*secondexpressionnode=
				firstparameternode->
				getNextTagSibling(sqlreparser::_parameter)->
				getFirstTagChild(sqlreparser::_expression);
	if (secondexpressionnode->isNullNode()) {
		return true;
	}

	// get the interval qualifier node
	xmldomnode	*iqnode=secondexpressionnode->getFirstTagChild(
						sqlreparser::_interval_qualifier);
	if (iqnode->isNullNode()) {
		return true;
	}

	// get the interval qualifier start index
	const char	*startstr=iqnode->getAttributeValue(sqlreparser::_from);
	timeparts_t	start=TIMEPARTS_YEAR;
	while (start!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(startstr,timeparts[start])) {
		start=(timeparts_t)((uint16_t)start+1);
	}

	// get the interval qualifier end index
	const char	*endstr=iqnode->getAttributeValue(sqlreparser::_to);
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
	functionnode->setAttributeValue(sqlreparser::_value,"convert");

	// insert a varchar parameter
	xmldomnode	*newparameternode=sqlts->newNodeBefore(
						parametersnode,
						firstparameternode,
						sqlreparser::_parameter);
	xmldomnode	*newexpressionnode=sqlts->newNode(
						newparameternode,
						sqlreparser::_expression);
	sqlts->newNode(newexpressionnode,sqlreparser::_string_literal,"varchar");

	// delete the interval_qualifier node and replace it with a new
	// string_literal node with the replacement format string
	secondexpressionnode->deleteChild(iqnode);
	sqlts->newNode(secondexpressionnode,sqlreparser::_string_literal,"21");

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
						sqlreparser::_string_literal,
						prepend);
		sqlts->newNodeAfter(functionnode->getParent(),
						prependliteral,
						sqlreparser::_plus);
	}
	if (append) {
		xmldomnode	*plus=
			sqlts->newNodeAfter(functionnode->getParent(),
						functionnode,
						sqlreparser::_plus);
		sqlts->newNodeAfter(functionnode->getParent(),
						plus,
						sqlreparser::_string_literal,
						append);
	}
}

bool informixtomssqlserverdate::translateCurrentDate(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldomnode *node) {

	// "function" -> sysdatetime()
	// or
	// "function" interval_qualifier -> sysdatetime()
	// ...

	debugFunction();

	// translate the current-date function to sysdatetime
	node->setAttributeValue(sqlreparser::_value,"sysdatetime");

	// add an empty parameter set
	sqlts->newNode(node,sqlreparser::_parameters);

	// get the interval qualifier node, if there is one...
	xmldomnode	*iqnode=node->getNextTagSibling();
	if (iqnode->isNullNode() ||
		charstring::compare(iqnode->getName(),
					sqlreparser::_interval_qualifier)) {

		// if there was no interval qualifier then just wrap
		// everything with a function to convert to a datetime
		// (as opposed to datetime2 which no math can be done on)
		wrapConvert(node,"datetime","21");
		return true;
	}

	// get the interval qualifier start index
	const char	*startstr=iqnode->getAttributeValue(sqlreparser::_from);
	timeparts_t	start=TIMEPARTS_YEAR;
	while (start!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(startstr,timeparts[start])) {
		start=(timeparts_t)((uint16_t)start+1);
	}

	// get the interval qualifier end index
	const char	*endstr=iqnode->getAttributeValue(sqlreparser::_to);
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

bool informixtomssqlserverdate::translateDateTime(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldomnode *node) {

	// datetime(...) interval_qualifier -> convert(datetime, ..., format)
	// ...

	debugFunction();

	// get the function node
	xmldomnode	*functionnode=node;

	// get the parameters node, if there is one...
	xmldomnode	*parametersnode=
				functionnode->getFirstTagChild(
						sqlreparser::_parameters);
	if (parametersnode->isNullNode()) {
		return true;
	}

	// get the first parameter node, if there is one...
	xmldomnode	*firstparameternode=
				parametersnode->getFirstTagChild(
						sqlreparser::_parameter);
	if (firstparameternode->isNullNode()) {
		return true;
	}

	// get the first parameter string literal node, if there is one...
	xmldomnode	*stringliteralnode=
				firstparameternode->getFirstTagChild(
						sqlreparser::_string_literal);
	if (stringliteralnode->isNullNode()) {
		return true;
	}

	// get the interval qualifier node, if there is one...
	xmldomnode	*iqnode=functionnode->getNextTagSibling(
					sqlreparser::_interval_qualifier);

	// translate datetime function to convert
	functionnode->setAttributeValue(sqlreparser::_value,"convert");

	// insert a datetime parameter
	xmldomnode	*newparameternode=sqlts->newNodeBefore(
						parametersnode,
						firstparameternode,
						sqlreparser::_parameter);
	xmldomnode	*newexpressionnode=sqlts->newNode(
						newparameternode,
						sqlreparser::_expression);
	sqlts->newNode(newexpressionnode,sqlreparser::_string_literal,"datetime");

	// update the date/time string
	// (we'll use sql server's format "21" (yyyy-mm-dd hh:mm:ss.fff) to
	// parse the datetime but we need to reformat it to match that as
	// closely as possible first)
	stringbuffer	datetimestring;
	translateDateTimeString(stringliteralnode->
				getAttributeValue(sqlreparser::_value),
				&datetimestring,iqnode);
	stringliteralnode->setAttributeValue(sqlreparser::_value,
						datetimestring.getString());

	// delete the interval_qualifier node
	functionnode->getParent()->deleteChild(iqnode);

	// add a format string parameter
	newparameternode=sqlts->newNodeAfter(parametersnode,
						firstparameternode,
						sqlreparser::_parameter);
	newexpressionnode=sqlts->newNode(newparameternode,
						sqlreparser::_expression);
	sqlts->newNode(newexpressionnode,sqlreparser::_string_literal,"21");

	return true;
}

bool informixtomssqlserverdate::translateInterval(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldomnode *node) {
	// FIXME... sqlserver doesn't support intervals, use DATEADD?
	// for now, don't do anything here
	debugFunction();
	return true;
}

void informixtomssqlserverdate::translateDateTimeString(
					const char *indtstring,
					stringbuffer *outdtstring,
					xmldomnode *iqnode) {

	// if there was no interval qualifier then just quote the string
	if (iqnode->isNullNode()) {
		outdtstring->append('\'');
		outdtstring->append(indtstring);
		outdtstring->append('\'');
		return;
	}

	// get the interval qualifier start index
	const char	*startstr=iqnode->getAttributeValue(sqlreparser::_from);
	timeparts_t	start=TIMEPARTS_YEAR;
	while (start!=TIMEPARTS_NULL &&
		charstring::compareIgnoringCase(startstr,timeparts[start])) {
		start=(timeparts_t)((uint16_t)start+1);
	}

	// get the interval qualifier end index
	const char	*endstr=iqnode->getAttributeValue(sqlreparser::_to);
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
						sqlreparser::_function,
						"substring");
	xmldomnode	*substringparameters=sqlts->newNode(
						substringnode,
						sqlreparser::_parameters);
	xmldomnode	*substringparameter1=sqlts->newNode(
						substringparameters,
						sqlreparser::_parameter);
	xmldomnode	*substringexpression1=sqlts->newNode(
						substringparameter1,
						sqlreparser::_expression);
	functionnode->getParent()->moveChild(functionnode,
						substringexpression1,0);
	xmldomnode	*substringparameter2=sqlts->newNode(
						substringparameters,
						sqlreparser::_parameter);
	xmldomnode	*substringexpression2=sqlts->newNode(
						substringparameter2,
						sqlreparser::_expression);
	char	*startstr=charstring::parseNumber(start);
	sqlts->newNode(substringexpression2,
				sqlreparser::_string_literal,startstr);
	delete[] startstr;
	xmldomnode	*substringparameter3=sqlts->newNode(
						substringparameters,
						sqlreparser::_parameter);
	xmldomnode	*substringexpression3=sqlts->newNode(
						substringparameter3,
						sqlreparser::_expression);
	char	*lengthstr=charstring::parseNumber(length);
	sqlts->newNode(substringexpression3,
				sqlreparser::_string_literal,lengthstr);
	delete[] lengthstr;
	return substringnode;
}

xmldomnode *informixtomssqlserverdate::wrapConvert(xmldomnode *functionnode,
						const char *datatype,
						const char *formatstring) {

	xmldomnode	*convertnode=sqlts->newNodeBefore(
						functionnode->getParent(),
						functionnode,
						sqlreparser::_function,
						"convert");
	xmldomnode	*convertparameters=sqlts->newNode(
						convertnode,
						sqlreparser::_parameters);
	xmldomnode	*convertparameter1=sqlts->newNode(
						convertparameters,
						sqlreparser::_parameter);
	xmldomnode	*convertexpression1=sqlts->newNode(
						convertparameter1,
						sqlreparser::_expression);
	sqlts->newNode(convertexpression1,
				sqlreparser::_string_literal,datatype);
	xmldomnode	*convertparameter2=sqlts->newNode(
						convertparameters,
						sqlreparser::_parameter);
	xmldomnode	*convertexpression2=sqlts->newNode(
						convertparameter2,
						sqlreparser::_expression);
	functionnode->getParent()->moveChild(functionnode,convertexpression2,0);
	xmldomnode	*convertparameter3=sqlts->newNode(
						convertparameters,
						sqlreparser::_parameter);
	xmldomnode	*convertexpression3=sqlts->newNode(
						convertparameter3,
						sqlreparser::_expression);
	sqlts->newNode(convertexpression3,
				sqlreparser::_string_literal,formatstring);
	return convertnode;
}

extern "C" {
	sqlrtranslation	*new_informixtomssqlserverdate(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new informixtomssqlserverdate(sqlts,parameters,debug);
	}
}
