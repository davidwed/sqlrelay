// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrerrortranslation_renumber :
					public sqlrerrortranslation {
	public:
			sqlrerrortranslation_renumber(
						sqlrservercontroller *cont,
						sqlrerrortranslations *sqlts,
						domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					int64_t errornumber,
					const char *error,
					uint32_t errorlength,
					int64_t *translatederrornumber,
					const char **translatederror,
					uint32_t *translatederrorlength);
	private:
		dictionary<int64_t,int64_t>	map;

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

	for (domnode *node=parameters->getFirstTagChild("renumber");
		!node->isNullNode(); node=node->getNextTagSibling("renumber")) {
		const char	*from=node->getAttributeValue("from");
		const char	*to=node->getAttributeValue("to");
		if (!charstring::isNullOrEmpty(from) &&
				!charstring::isNullOrEmpty(to)) {
			map.setValue(charstring::toInteger(from),
					charstring::toInteger(to));
		}
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

	*translatederrornumber=errornumber;
	*translatederror=error;
	*translatederrorlength=errorlength;

	if (!enabled) {
		return true;
	}

	if (debug) {
		stdoutput.printf("original error number:\n\"%lld\"\n\n",
								errornumber);
	}

	int64_t	to;
	if (map.getValue(errornumber,&to)) {
		*translatederrornumber=to;
	}

	if (debug) {
		stdoutput.printf("translated to:\n\"%lld\"\n\n",
							*translatederrornumber);
	}
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
