// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltranslations/translatedatetimes.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>

translatedatetimes::translatedatetimes(sqltranslations *sqlts,
					xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool translatedatetimes::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	if (sqlrcon->debugsqltranslation) {
		printf("date/time translation:\n");
		printf("    ddmm: %s\n",
			parameters->getAttributeValue("ddmm"));
		printf("    datetime: %s\n",
			parameters->getAttributeValue("datetime"));
		printf("    date: %s\n",
			parameters->getAttributeValue("date"));
		printf("    time: %s\n",
			parameters->getAttributeValue("time"));
		printf("  binds:\n");
	}
	if (!translateDateTimesInBindVariables(sqlrcon,sqlrcur,
						querytree->getRootNode(),
						parameters)) {
		return false;
	}
	if (sqlrcon->debugsqltranslation) {
		printf("  query:\n");
	}
	if (!translateDateTimesInQuery(sqlrcon,sqlrcur,
						querytree->getRootNode(),
						parameters)) {
		return false;
	}
	if (sqlrcon->debugsqltranslation) {
		printf("\n");
	}
	return true;
}

#include <parsedatetime.h>
char *translatedatetimes::convertDateTime(const char *format,
			int16_t year, int16_t month, int16_t day,
			int16_t hour, int16_t minute, int16_t second) {

	// if no format was passed in
	if (!format) {
		return NULL;
	}

	// output buffer
	stringbuffer	output;

	// work buffer
	char		buf[5];

	// run through the format string
	const char	*ptr=format;
	while (*ptr) {

		if (!charstring::compare(ptr,"DD",2)) {
			snprintf(buf,5,"%02d",day);
			output.append(buf);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"MM",2)) {
			snprintf(buf,5,"%02d",month);
			output.append(buf);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"MON",3)) {
			if (month>0) {
				output.append(shortmonths[month-1]);
			}
			ptr=ptr+3;
		} else if (!charstring::compare(ptr,"Month",5)) {
			if (month>0) {
				output.append(longmonths[month-1]);
			}
			ptr=ptr+3;
		} else if (!charstring::compare(ptr,"YYYY",4)) {
			snprintf(buf,5,"%04d",year);
			output.append(buf);
			ptr=ptr+4;
		} else if (!charstring::compare(ptr,"YY",2)) {
			snprintf(buf,5,"%04d",year);
			output.append(buf+2);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"HH24",4)) {
			snprintf(buf,5,"%02d",hour);
			output.append(buf);
			ptr=ptr+4;
		} else if (!charstring::compare(ptr,"HH",2)) {
			snprintf(buf,5,"%02d",(hour<13)?hour:hour-12);
			output.append(buf);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"MI",2)) {
			snprintf(buf,5,"%02d",minute);
			output.append(buf);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"SS",2)) {
			snprintf(buf,5,"%02d",second);
			output.append(buf);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"AM",2)) {
			output.append((hour<13)?"AM":"PM");
			ptr=ptr+2;
		} else {
			output.append(*ptr);
			ptr=ptr+1;
		}
	}

	return output.detachString();
}

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
	
			// parse the date/time
			if (parseDateTime(valuecopy,ddmm,false,
						&year,&month,&day,
						&hour,&minute,&second)) {

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
							hour,minute,second);
				if (converted) {

					if (sqlrcon->debugsqltranslation) {
						printf("    %s -> %s\n",
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
	
		// parse the date/time
		if (!parseDateTime(bind->value.stringval,ddmm,false,
						&year,&month,&day,
						&hour,&minute,&second)) {
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
							hour,minute,second);
		if (!converted) {
			continue;
		}

		if (sqlrcon->debugsqltranslation) {
			printf("    %s -> %s\n",
				bind->value.stringval,converted);
		}

		// replace the value with the converted string
		bind->valuesize=charstring::length(converted);
		bind->value.stringval=
			(char *)sqlrcon->bindmappingspool->
					calloc(bind->valuesize+1);
		charstring::copy(bind->value.stringval,converted);
		delete[] converted;
	}

	return true;
}

const char * const *translatedatetimes::getShortMonths() {
	return shortmonths;
}

const char * const *translatedatetimes::getLongMonths() {
	return longmonths;
}
