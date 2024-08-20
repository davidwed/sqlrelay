// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrerrortranslation_renumber :
					public sqlrerrortranslation {
	public:
		sqlrerrortranslation_renumber(sqlrservercontroller *cont,
						domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					int64_t errornumber,
					const char *error,
					uint32_t errorlength,
					int64_t *translatederrornumber,
					stringbuffer *translatederror);
	private:
		dictionary<int64_t,int64_t>	map;
};

sqlrerrortranslation_renumber::sqlrerrortranslation_renumber(
						sqlrservercontroller *cont,
						domnode *parameters) :
				sqlrerrortranslation(cont,parameters) {
	debugFunction();

	for (domnode *node=parameters->getFirstTagChild("renumber");
		!node->isNullNode(); node=node->getNextTagSibling("renumber")) {
		const char	*from=node->getAttributeValue("from");
		const char	*to=node->getAttributeValue("to");
		if (!charstring::isNullOrEmpty(from) &&
				!charstring::isNullOrEmpty(to)) {
			map.setValue(charstring::convertToInteger(from),
					charstring::convertToInteger(to));
		}
	}
}

bool sqlrerrortranslation_renumber::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					int64_t errornumber,
					const char *error,
					uint32_t errorlength,
					int64_t *translatederrornumber,
					stringbuffer *translatederror) {
	debugFunction();

	*translatederrornumber=errornumber;
	translatederror->append(error,errorlength);

	debugWrite("original error number:");
	debugWrite("\"%lld\"",errornumber);

	int64_t	to;
	if (map.getValue(errornumber,&to)) {
		*translatederrornumber=to;
	}

	debugWrite("translated to:");
	debugWrite("\"%lld\"",*translatederrornumber);

	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrerrortranslation
			*new_sqlrerrortranslation_renumber(
						sqlrservercontroller *cont,
						domnode *parameters) {
		return new sqlrerrortranslation_renumber(cont,parameters);
	}
}
