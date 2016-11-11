// Copyright (c) 2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrfilterprivate {
	friend class sqlrfilter;
	private:
		sqlrfilters	*_fs;
		xmldomnode	*_parameters;
};

sqlrfilter::sqlrfilter(sqlrservercontroller *cont,
				sqlrfilters *fs,
				xmldomnode *parameters) {
	pvt=new sqlrfilterprivate;
	pvt->_fs=fs;
	pvt->_parameters=parameters;
}

sqlrfilter::~sqlrfilter() {
	delete pvt;
}

bool sqlrfilter::usesTree() {
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
			pvt->_parameters->getAttributeValue("error");
	const char	*errnum=
			pvt->_parameters->getAttributeValue("errornumber");
	if (err) {
		if (!charstring::isNullOrEmpty(error)) {
			*err=error;
		} else {
			*err=NULL;
		}
	}
	if (errn) {
		if (!charstring::isNullOrEmpty(errnum)) {
			*errn=charstring::toInteger(errnum);
		} else {
			*errn=0;
		}
	}
}

sqlrfilters *sqlrfilter::getFilters() {
	return pvt->_fs;
}

xmldomnode *sqlrfilter::getParameters() {
	return pvt->_parameters;
}
