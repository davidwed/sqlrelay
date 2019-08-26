// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/regularexpression.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrfilter_regex : public sqlrfilter {
	public:
			sqlrfilter_regex(sqlrservercontroller *cont,
						sqlrfilters *fs,
						domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);
	private:
		regularexpression	re;
		const char		*pattern;

		bool	enabled;

		bool	debug;
};

sqlrfilter_regex::sqlrfilter_regex(sqlrservercontroller *cont,
						sqlrfilters *fs,
						domnode *parameters) :
						sqlrfilter(cont,fs,parameters) {
	debugFunction();

	debug=cont->getConfig()->getDebugFilters();

	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));
	if (!enabled) {
		return;
	}

	pattern=parameters->getAttributeValue("pattern");
	re.setPattern(pattern);
	re.study();
}

bool sqlrfilter_regex::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				const char *query) {
	debugFunction();

	if (!enabled) {
		return true;
	}

	if (re.match(query)) {
		if (debug) {
			stdoutput.printf("regex: matches pattern \"%s\"\n\n",
								pattern);
		}
		return false;
	}
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrfilter *new_sqlrfilter_regex(
						sqlrservercontroller *cont,
						sqlrfilters *fs,
						domnode *parameters) {
		return new sqlrfilter_regex(cont,fs,parameters);
	}
}
