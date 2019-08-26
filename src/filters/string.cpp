// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/character.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrfilter_string : public sqlrfilter {
	public:
			sqlrfilter_string(sqlrservercontroller *cont,
						sqlrfilters *fs,
						domnode *parameters);
			~sqlrfilter_string();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);
	private:
		const char		*pattern;
		char			*lowerpattern;
		bool			ignorecase;

		bool	enabled;

		bool	debug;
};

sqlrfilter_string::sqlrfilter_string(sqlrservercontroller *cont,
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

	ignorecase=charstring::isYes(
			parameters->getAttributeValue("ignorecase"));

	lowerpattern=NULL;
	if (ignorecase) {
		lowerpattern=charstring::duplicate(pattern);
		for (char *c=lowerpattern; *c; c++) {
			*c=character::toLowerCase(*c);
		}
		pattern=lowerpattern;
	}
}

sqlrfilter_string::~sqlrfilter_string() {
	delete[] lowerpattern;
}

bool sqlrfilter_string::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query) {
	debugFunction();

	if (!enabled) {
		return true;
	}

	char	*lowered=NULL;
	if (ignorecase) {
		lowered=charstring::duplicate(query);
		for (char *c=lowered; *c; c++) {
			*c=character::toLowerCase(*c);
		}
		query=lowered;
	}

	bool	result=!charstring::contains(query,pattern);

	if (result && debug) {
		stdoutput.printf("string: matches pattern \"%s\"\n\n",pattern);
	}

	delete[] lowered;
	return result;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrfilter
			*new_sqlrfilter_string(sqlrservercontroller *cont,
						sqlrfilters *fs,
						domnode *parameters) {
		return new sqlrfilter_string(cont,fs,parameters);
	}
}
