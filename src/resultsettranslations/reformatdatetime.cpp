// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrresultsettranslation_reformatdatetime :
					public sqlrresultsettranslation {
	public:
		sqlrresultsettranslation_reformatdatetime(
					sqlrservercontroller *cont,
					domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint32_t fieldindex,
					const char **field,
					uint64_t *fieldsize);
	private:
		bool		ddmm;
		bool		yyyyddmm;
		bool		ignorenondatetime;
		const char	*datedelimiters;
		const char	*datetimeformat;
		const char	*dateformat;
		const char	*timeformat;
};

sqlrresultsettranslation_reformatdatetime::
	sqlrresultsettranslation_reformatdatetime(
				sqlrservercontroller *cont,
				domnode *parameters) :
				sqlrresultsettranslation(cont,parameters) {

	// get the parameters
	const char	*dateddmm=
			parameters->getAttributeValue("dateddmm");
	const char	*dateyyyyddmm=
			parameters->getAttributeValue("dateyyyyddmm");
	if (charstring::getLength(dateddmm) &&
		!charstring::getLength(dateyyyyddmm)) {
		dateyyyyddmm=dateddmm;
	}
	ddmm=charstring::isYes(dateddmm);
	yyyyddmm=charstring::isYes(dateyyyyddmm);

	ignorenondatetime=charstring::isYes(
				parameters->getAttributeValue(
						"ignorenondatetime"));

	datedelimiters=parameters->getAttributeValue("datedelimiters");
	if (!datedelimiters) {
		datedelimiters="/-.:";
	}

	datetimeformat=parameters->getAttributeValue("datetimeformat");
	dateformat=parameters->getAttributeValue("dateformat");
	timeformat=parameters->getAttributeValue("timeformat");

}

bool sqlrresultsettranslation_reformatdatetime::run(
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint32_t fieldindex,
					const char **field,
					uint64_t *fieldsize) {
	debugFunction();

	// ignore non-date fields, if specified
	if (ignorenondatetime &&
		!sqlrcon->cont->isDateTimeType(
			sqlrcon->cont->getColumnType(sqlrcur,fieldindex))) {
		return true;
	}

	// This weirdness is mainly to address a FreeTDS/MSSQL
	// issue.  See the code for the method
	// freetdscursor::ignoreDateDdMmParameter() for more info.
	bool	localddmm=ddmm;
	bool	localyyyyddmm=yyyyddmm;
	if (sqlrcur->ignoreDateDdMmParameter(*field,*fieldsize)) {
		localddmm=false;
		localyyyyddmm=false;
	}

	// reformat the date/time
	sqlrcon->cont->reformatDateTime(*field,
					*fieldsize,
					field,
					fieldsize,
					localddmm,
					localyyyyddmm,
					datedelimiters,
					datetimeformat,
					dateformat,
					timeformat);

	debugWrite("using ddmm=%d and yyyyddmm=%d",ddmm,yyyyddmm);

	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrresultsettranslation
			*new_sqlrresultsettranslation_reformatdatetime(
					sqlrservercontroller *cont,
					domnode *parameters) {
		return new sqlrresultsettranslation_reformatdatetime(
							cont,parameters);
	}
}
