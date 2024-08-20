// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/character.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrfilter_string : public sqlrfilter {
	public:
		sqlrfilter_string(sqlrservercontroller *cont,
						domnode *parameters);
		~sqlrfilter_string();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);
	private:
		const char	*pattern;
		char		*lowerpattern;
		bool		ignorecase;
};

sqlrfilter_string::sqlrfilter_string(sqlrservercontroller *cont,
						domnode *parameters) :
						sqlrfilter(cont,parameters) {
	debugFunction();

	pattern=parameters->getAttributeValue("pattern");

	ignorecase=charstring::isYes(
			parameters->getAttributeValue("ignorecase"));

	lowerpattern=NULL;
	if (ignorecase) {
		lowerpattern=charstring::duplicate(pattern);
		for (char *c=lowerpattern; *c; c++) {
			*c=character::lower(*c);
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

	char	*lowered=NULL;
	if (ignorecase) {
		lowered=charstring::duplicate(query);
		for (char *c=lowered; *c; c++) {
			*c=character::lower(*c);
		}
		query=lowered;
	}

	bool	result=!charstring::contains(query,pattern);

	if (!result) {
		debugWrite("string: matches pattern \"%s\"",pattern);
	}

	delete[] lowered;
	return result;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrfilter
			*new_sqlrfilter_string(sqlrservercontroller *cont,
						domnode *parameters) {
		return new sqlrfilter_string(cont,parameters);
	}
}
