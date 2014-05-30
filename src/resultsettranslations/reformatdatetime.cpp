// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrcontroller.h>
#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrresultsettranslation.h>
#define NEED_CONVERT_DATE_TIME
#include <parsedatetime.h>
#include <debugprint.h>

class reformatdatetime : public sqlrresultsettranslation {
	public:
			reformatdatetime(sqlrresultsettranslations *sqlrrsts,
							xmldomnode *parameters);
			~reformatdatetime();
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					uint32_t fieldindex,
					const char *field,
					uint64_t fieldlength,
					const char **newfield,
					uint64_t *newfieldlength);
	private:
		char		*reformattedfield;
		uint32_t	reformattedfieldlength;
};

reformatdatetime::reformatdatetime(sqlrresultsettranslations *sqlrrsts,
						xmldomnode *parameters) :
				sqlrresultsettranslation(sqlrrsts,parameters) {
	reformattedfield=NULL;
	reformattedfieldlength=0;
}

reformatdatetime::~reformatdatetime() {
	delete[] reformattedfield;
}

bool reformatdatetime::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					uint32_t fieldindex,
					const char *field,
					uint64_t fieldlength,
					const char **newfield,
					uint64_t *newfieldlength) {
	debugFunction();

	// initialize return values
	*newfield=field;
	*newfieldlength=fieldlength;

	// are dates going to be in MM/DD or DD/MM format?
	bool	ddmm=sqlrcon->cont->cfgfl->getDateDdMm();
	bool	yyyyddmm=sqlrcon->cont->cfgfl->getDateYyyyDdMm();

	// This weirdness is mainly to address a FreeTDS/MSSQL
	// issue.  See the code for the method
	// freetdscursor::ignoreDateDdMmParameter() for more info.
	if (sqlrcur->ignoreDateDdMmParameter(fieldindex,field,fieldlength)) {
		ddmm=false;
		yyyyddmm=false;
	}

	int16_t	year=-1;
	int16_t	month=-1;
	int16_t	day=-1;
	int16_t	hour=-1;
	int16_t	minute=-1;
	int16_t	second=-1;
	int16_t	fraction=-1;
	if (!parseDateTime(field,ddmm,yyyyddmm,true,
				&year,&month,&day,
				&hour,&minute,&second,
				&fraction)) {
		return true;
	}

	// decide which format to use based on what parts
	// were detected in the date/time
	const char	*format=sqlrcon->cont->cfgfl->getDateTimeFormat();
	if (hour==-1) {
		format=sqlrcon->cont->cfgfl->getDateFormat();
	} else if (day==-1) {
		format=sqlrcon->cont->cfgfl->getTimeFormat();
	}

	// convert to the specified format
	delete[] reformattedfield;
	reformattedfield=convertDateTime(format,
					year,month,day,
					hour,minute,second,
					fraction);
	reformattedfieldlength=charstring::length(reformattedfield);

	if (sqlrcon->cont->debugsqlrtranslation) {
		stdoutput.printf("converted date: "
			"\"%s\" to \"%s\" using ddmm=%d\n",
			field,reformattedfield,ddmm);
	}

	// set return values
	*newfield=reformattedfield;
	*newfieldlength=reformattedfieldlength;

	return true;
}

extern "C" {
	sqlrresultsettranslation *new_reformatdatetime(
					sqlrresultsettranslations *sqlrrsts,
					xmldomnode *parameters) {
		return new reformatdatetime(sqlrrsts,parameters);
	}
}
