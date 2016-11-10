// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrtranslationprivate {
	friend class sqlrtranslation;
	private:
		sqlrtranslations	*_sqlts;
		xmldomnode		*_parameters;
};

sqlrtranslation::sqlrtranslation(sqlrservercontroller *cont,
					sqlrtranslations *sqlts,
					xmldomnode *parameters) {
	pvt=new sqlrtranslationprivate;
	pvt->_sqlts=sqlts;
	pvt->_parameters=parameters;
}

sqlrtranslation::~sqlrtranslation() {
	delete pvt;
}

bool sqlrtranslation::usesTree() {
	return false;
}

bool sqlrtranslation::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				const char *query,
				stringbuffer *translatedquery) {
	return true;
}

bool sqlrtranslation::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				xmldom *querytree) {
	return true;
}

sqlrtranslations *sqlrtranslation::getTranslations() {
	return pvt->_sqlts;
}

xmldomnode *sqlrtranslation::getParameters() {
	return pvt->_parameters;
}
