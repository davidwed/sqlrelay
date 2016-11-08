// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrtranslationprivate {
	friend class sqlrtranslation;
	private:
		sqlrtranslations	*_sqlts;
		xmldomnode		*_parameters;
		bool			_debug;
};

sqlrtranslation::sqlrtranslation(sqlrtranslations *sqlts,
				xmldomnode *parameters,
				bool debug) {
	pvt=new sqlrtranslationprivate;
	pvt->_sqlts=sqlts;
	pvt->_parameters=parameters;
	pvt->_debug=debug;
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

bool sqlrtranslation::getDebug() {
	return pvt->_debug;
}
