// Copyright (c) 2015  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

sqlrfilter::sqlrfilter(sqlrfilters *sqlrfs,
				xmldomnode *parameters,
				bool debug) {
	this->sqlrfs=sqlrfs;
	this->parameters=parameters;
	this->debug=debug;
}

sqlrfilter::~sqlrfilter() {
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
	const char	*error=parameters->getAttributeValue("error");
	const char	*errno=parameters->getAttributeValue("errornumber");
	if (err) {
		if (!charstring::isNullOrEmpty(error)) {
			*err=error;
		} else {
			*err=NULL;
		}
	}
	if (errn) {
		if (!charstring::isNullOrEmpty(errno)) {
			*errn=charstring::toInteger(errno);
		} else {
			*errn=0;
		}
	}
}
