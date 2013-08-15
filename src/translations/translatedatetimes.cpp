// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslation.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class translatedatetimes : public sqltranslation {
	public:
			translatedatetimes(sqltranslations *sqlts,
					xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		bool translateDateTimesInQuery(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *querynode,
					xmldomnode *parameters);
		bool translateDateTimesInBindVariables(
					sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *querynode,
					xmldomnode *parameters);
};

translatedatetimes::translatedatetimes(sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool translatedatetimes::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	if (sqlrcon->cont->debugsqltranslation) {
		stdoutput.printf("date/time translation:\n");
		stdoutput.printf("    ddmm: %s\n",
			parameters->getAttributeValue("ddmm"));
		stdoutput.printf("    datetime: %s\n",
			parameters->getAttributeValue("datetime"));
		stdoutput.printf("    date: %s\n",
			parameters->getAttributeValue("date"));
		stdoutput.printf("    time: %s\n",
			parameters->getAttributeValue("time"));
		stdoutput.printf("  binds:\n");
	}
	if (!translateDateTimesInBindVariables(sqlrcon,sqlrcur,
						querytree->getRootNode(),
						parameters)) {
		return false;
	}
	if (sqlrcon->cont->debugsqltranslation) {
		stdoutput.printf("  query:\n");
	}
	if (!translateDateTimesInQuery(sqlrcon,sqlrcur,
						querytree->getRootNode(),
						parameters)) {
		return false;
	}
	if (sqlrcon->cont->debugsqltranslation) {
		stdoutput.printf("\n");
	}
	return true;
}

#define NEED_CONVERT_DATE_TIME
#include <parsedatetime.h>

bool translatedatetimes::translateDateTimesInQuery(
					sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *querynode,
					xmldomnode *parameters) {
	debugFunction();
	
	// input format
	bool		ddmm=!charstring::compare(
				parameters->getAttributeValue("ddmm"),
				"yes");

	// output format
	const char	*datetimeformat=
			parameters->getAttributeValue("datetime");
	const char	*dateformat=
			parameters->getAttributeValue("date");
	const char	*timeformat=
			parameters->getAttributeValue("time");
	if (!datetimeformat) {
		datetimeformat="DD-MON-YYYY HH24:MI:SS";
	}
	if (!dateformat) {
		dateformat="DD-MON-YYYY";
	}
	if (!timeformat) {
		timeformat="HH24:MI:SS";
	}

	// convert this node...
	if (!charstring::compare(querynode->getName(),
					sqlparser::_verbatim) ||
		!charstring::compare(querynode->getName(),
					sqlparser::_value) ||
		!charstring::compare(querynode->getName(),
					sqlparser::_string_literal)) {

		// get the value
		const char	*value=querynode->getAttributeValue(
							sqlparser::_value);

		// leave it alone unless it's a string
		// NOTE: This is important to do... In particular, the informix
		// datetime() function takes a non-quoted date string that must
		// be YYYY-MM-DD HH24-MI-SS.FF and must not be translated into
		// a different format but gets stored in a string_literal node,
		// at least for now.  Verifying that anything that will be
		// translated starts and ends with quotes prevents these from
		// being translated.
		if (sqlts->isString(value)) {

			// copy it and strip off the quotes
			char	*valuecopy=charstring::duplicate(value+1);
			valuecopy[charstring::length(valuecopy)-1]='\0';

			// variables
			int16_t	year=-1;
			int16_t	month=-1;
			int16_t	day=-1;
			int16_t	hour=-1;
			int16_t	minute=-1;
			int16_t	second=-1;
			int16_t	fraction=-1;
	
			// parse the date/time
			if (parseDateTime(valuecopy,ddmm,false,
						&year,&month,&day,
						&hour,&minute,&second,
						&fraction)) {

				// decide which format to use
				bool	validdate=(year!=-1 &&
						month!=-1 && day!=-1);
				bool	validtime=(hour!=-1 &&
						minute!=-1 && second!=-1);
				const char	*format=NULL;
				if (validdate && validtime) {
					format=datetimeformat;
				} else if (validdate) {
					format=dateformat;
				} else if (validtime) {
					format=timeformat;
				}

				// convert it
				char	*converted=convertDateTime(
							format,
							year,month,day,
							hour,minute,second,
							fraction);
				if (converted) {

					if (sqlrcon->cont->debugsqltranslation) {
						stdoutput.printf(
							"    %s -> %s\n",
							valuecopy,converted);
					}

					// repackage as a string
					stringbuffer	output;
					output.append('\'');
					output.append(converted);
					output.append('\'');

					// update the value
					sqlts->setAttribute(querynode,
							sqlparser::_value,
							output.getString());

					// clean up
					delete[] converted;
				}
			}
	
			// clean up
			delete[] valuecopy;
		}
	}

	// convert child nodes...
	for (xmldomnode *node=querynode->getFirstTagChild();
			!node->isNullNode(); node=node->getNextTagSibling()) {
		if (!translateDateTimesInQuery(sqlrcon,sqlrcur,
						node,parameters)) {
			return false;
		}
	}
	return true;
}

bool translatedatetimes::translateDateTimesInBindVariables(
						sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldomnode *querynode,
						xmldomnode *parameters) {
	debugFunction();

	// input format
	bool		ddmm=!charstring::compare(
				parameters->getAttributeValue("ddmm"),
				"yes");

	// output format
	const char	*datetimeformat=
			parameters->getAttributeValue("datetime");
	const char	*dateformat=
			parameters->getAttributeValue("date");
	const char	*timeformat=
			parameters->getAttributeValue("time");
	if (!datetimeformat) {
		datetimeformat="DD-MON-YYYY HH24:MI:SS";
	}
	if (!dateformat) {
		dateformat="DD-MON-YYYY";
	}
	if (!timeformat) {
		timeformat="HH24:MI:SS";
	}

	// run through the bind variables...
	for (uint16_t i=0; i<sqlrcur->inbindcount; i++) {

		// get the variable
		bindvar_svr	*bind=&(sqlrcur->inbindvars[i]);

		// ignore non-strings...
		if (bind->type!=STRING_BIND) {
			continue;
		}

		// variables
		int16_t	year=-1;
		int16_t	month=-1;
		int16_t	day=-1;
		int16_t	hour=-1;
		int16_t	minute=-1;
		int16_t	second=-1;
		int16_t fraction=-1;
	
		// parse the date/time
		if (!parseDateTime(bind->value.stringval,ddmm,false,
						&year,&month,&day,
						&hour,&minute,&second,
						&fraction)) {
			continue;
		}

		// decide which format to use
		bool	validdate=(year!=-1 && month!=-1 && day!=-1);
		bool	validtime=(hour!=-1 && minute!=-1 && second!=-1);
		const char	*format=NULL;
		if (validdate && validtime) {
			format=datetimeformat;
		} else if (validdate) {
			format=dateformat;
		} else if (validtime) {
			format=timeformat;
		}

		// attempt to convert the value
		char	*converted=convertDateTime(format,
							year,month,day,
							hour,minute,second,
							fraction);
		if (!converted) {
			continue;
		}

		if (sqlrcon->cont->debugsqltranslation) {
			stdoutput.printf("    %s -> %s\n",
					bind->value.stringval,converted);
		}

		// replace the value with the converted string
		bind->valuesize=charstring::length(converted);
		bind->value.stringval=
			(char *)sqlrcon->cont->bindmappingspool->
						calloc(bind->valuesize+1);
		charstring::copy(bind->value.stringval,converted);
		delete[] converted;
	}

	return true;
}

extern "C" {
	sqltranslation	*new_translatedatetimes(
					sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new translatedatetimes(sqlts,parameters);
	}
}
