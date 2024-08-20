// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrfilterprivate {
	friend class sqlrfilter;
	private:
};

sqlrfilter::sqlrfilter(sqlrservercontroller *cont,
				domnode *parameters) :
				sqlrservermodule(cont,parameters) {
	pvt=new sqlrfilterprivate;
}

sqlrfilter::~sqlrfilter() {
	delete pvt;
}

bool sqlrfilter::requiresTree() {
	return false;
}

bool sqlrfilter::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				const char *query) {
	return true;
}

bool sqlrfilter::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				xmldom *querytree) {
	return true;
}

void sqlrfilter::getError(const char **err, int64_t *errn) {
	const char	*error=
			getParameters()->getAttributeValue("error");
	const char	*errnum=
			getParameters()->getAttributeValue("errornumber");
	if (err) {
		if (!charstring::isNullOrEmpty(error)) {
			*err=error;
		} else {
			*err=NULL;
		}
	}
	if (errn) {
		if (!charstring::isNullOrEmpty(errnum)) {
			*errn=charstring::convertToInteger(errnum);
		} else {
			*errn=0;
		}
	}
}
