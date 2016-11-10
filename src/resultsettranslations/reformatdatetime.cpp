// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrresultsettranslation_reformatdatetime :
					public sqlrresultsettranslation {
	public:
			sqlrresultsettranslation_reformatdatetime(
					sqlrservercontroller *cont,
					sqlrresultsettranslations *sqlrrsts,
					xmldomnode *parameters);
			~sqlrresultsettranslation_reformatdatetime();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint32_t fieldindex,
					const char **field,
					uint64_t *fieldlength);
	private:
		char		*reformattedfield;
		uint64_t	reformattedfieldlength;

		bool		ddmm;
		bool		yyyyddmm;
		bool		ignorenondatetime;
		const char	*datedelimiters;
		const char	*datetimeformat;
		const char	*dateformat;
		const char	*timeformat;

		bool	enabled;

		bool	debug;
};

sqlrresultsettranslation_reformatdatetime::
	sqlrresultsettranslation_reformatdatetime(
				sqlrservercontroller *cont,
				sqlrresultsettranslations *sqlrrsts,
				xmldomnode *parameters) :
			sqlrresultsettranslation(cont,sqlrrsts,parameters) {

	debug=cont->getConfig()->getDebugResultSetTranslations();

	reformattedfield=NULL;
	reformattedfieldlength=0;

	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
	if (!enabled) {
		return;
	}

	// get the parameters
	const char	*dateddmm=
			parameters->getAttributeValue("dateddmm");
	const char	*dateyyyyddmm=
			parameters->getAttributeValue("dateyyyyddmm");
	if (charstring::length(dateddmm) &&
		!charstring::length(dateyyyyddmm)) {
		dateyyyyddmm=dateddmm;
	}
	ddmm=!charstring::compareIgnoringCase(dateddmm,"yes");
	yyyyddmm=!charstring::compareIgnoringCase(dateyyyyddmm,"yes");

	ignorenondatetime=!charstring::compareIgnoringCase(
				parameters->getAttributeValue(
						"ignorenondatetime"),"yes");

	datedelimiters=parameters->getAttributeValue("datedelimiters");
	if (!datedelimiters) {
		datedelimiters="/-.:";
	}

	datetimeformat=parameters->getAttributeValue("datetimeformat");
	dateformat=parameters->getAttributeValue("dateformat");
	timeformat=parameters->getAttributeValue("timeformat");

}

sqlrresultsettranslation_reformatdatetime::
	~sqlrresultsettranslation_reformatdatetime() {
	delete[] reformattedfield;
}

bool sqlrresultsettranslation_reformatdatetime::run(
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint32_t fieldindex,
					const char **field,
					uint64_t *fieldlength) {
	debugFunction();

	if (!enabled) {
		return true;
	}

	if (debug) {
		stdoutput.printf("converted date \"%s\" ",*field);
	}

	// For now, call the sqlrservercontroller method.
	// Eventually that code should be moved here.
	sqlrcon->cont->reformatDateTimes(sqlrcur,fieldindex,
					*field,*fieldlength,
					field,fieldlength,
					ddmm,yyyyddmm,
					ignorenondatetime,
					datedelimiters,
					datetimeformat,
					dateformat,
					timeformat);

	if (debug) {
		stdoutput.printf("\"%s\"\nusing ddmm=%d and yyyyddmm=%d\n",
						*field,ddmm,yyyyddmm);
	}

	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrresultsettranslation
			*new_sqlrresultsettranslation_reformatdatetime(
					sqlrservercontroller *cont,
					sqlrresultsettranslations *sqlrrsts,
					xmldomnode *parameters) {
		return new sqlrresultsettranslation_reformatdatetime(
						cont,sqlrrsts,parameters);
	}
}
