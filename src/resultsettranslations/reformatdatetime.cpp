// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <debugprint.h>

class SQLRSERVER_DLLSPEC reformatdatetime : public sqlrresultsettranslation {
	public:
			reformatdatetime(sqlrresultsettranslations *sqlrrsts,
							xmldomnode *parameters);
			~reformatdatetime();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint16_t fieldindex,
					const char *field,
					uint32_t fieldlength,
					const char **newfield,
					uint32_t *newfieldlength);
	private:
		char		*reformattedfield;
		uint32_t	reformattedfieldlength;

		bool		ddmm;
		bool		yyyyddmm;
		bool		ignorenondatetime;
		const char	*datedelimiters;
		const char	*datetimeformat;
		const char	*dateformat;
		const char	*timeformat;

		bool	enabled;
};

reformatdatetime::reformatdatetime(sqlrresultsettranslations *sqlrrsts,
						xmldomnode *parameters) :
				sqlrresultsettranslation(sqlrrsts,parameters) {

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

reformatdatetime::~reformatdatetime() {
	delete[] reformattedfield;
}

bool reformatdatetime::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint16_t fieldindex,
					const char *field,
					uint32_t fieldlength,
					const char **newfield,
					uint32_t *newfieldlength) {
	debugFunction();

	if (!enabled) {
		return true;
	}

	// For now, call the sqlrservercontroller method.
	// Eventually that code should be moved here.
	sqlrcon->cont->reformatDateTimes(sqlrcur,fieldindex,
					field,fieldlength,
					newfield,newfieldlength,
					ddmm,yyyyddmm,
					ignorenondatetime,
					datedelimiters,
					datetimeformat,
					dateformat,
					timeformat);

	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrresultsettranslation
			*new_sqlrresultsettranslation_reformatdatetime(
					sqlrresultsettranslations *sqlrrsts,
					xmldomnode *parameters) {
		return new reformatdatetime(sqlrrsts,parameters);
	}
}
