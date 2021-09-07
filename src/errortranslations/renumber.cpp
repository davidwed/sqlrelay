// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/regularexpression.h>
#include <rudiments/character.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrerrortranslation_renumber :
					public sqlrerrortranslation {
	public:
			sqlrerrortranslation_renumber(
						sqlrservercontroller *cont,
						sqlrerrortranslations *sqlts,
						domnode *parameters);
			~sqlrerrortranslation_renumber();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					int64_t errornumber,
					const char *error,
					uint32_t errorlength,
					int64_t *translatederrornumber,
					const char **translatederror,
					uint32_t *translatederrorlength);
	private:
		bool	enabled;

		bool	debug;
};

sqlrerrortranslation_renumber::sqlrerrortranslation_renumber(
						sqlrservercontroller *cont,
						sqlrerrortranslations *sqlts,
						domnode *parameters) :
				sqlrerrortranslation(cont,sqlts,parameters) {
	debugFunction();

	debug=cont->getConfig()->getDebugTranslations();

	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));
	if (!enabled) {
		return;
	}
}

bool sqlrerrortranslation_renumber::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					int64_t errornumber,
					const char *error,
					uint32_t errorlength,
					int64_t *translatederrornumber,
					const char **translatederror,
					uint32_t *translatederrorlength) {
	debugFunction();

	if (!enabled) {
		*translatederrornumber=errornumber;
		*translatederror=error;
		*translatederrorlength=errorlength;
		return true;
	}

	if (debug) {
		stdoutput.printf("original error number:\n\"%ld\"\n\n",
								errornumber);
	}

	// FIXME: renumber...
	*translatederrornumber=errornumber;
	*translatederror=error;
	*translatederrorlength=errorlength;

	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrerrortranslation
			*new_sqlrerrortranslation_renumber(
						sqlrservercontroller *cont,
						sqlrerrortranslations *ts,
						domnode *parameters) {
		return new sqlrerrortranslation_renumber(cont,ts,parameters);
	}
}
