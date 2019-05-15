// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrtranslationprivate {
	friend class sqlrtranslation;
	private:

		sqlrtranslations	*_sqlts;
		domnode		*_parameters;
};

sqlrtranslation::sqlrtranslation(sqlrservercontroller *cont,
					sqlrtranslations *sqlts,
					domnode *parameters) {
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
				uint32_t querylength,
				stringbuffer *translatedquery) {
	return true;
}

bool sqlrtranslation::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				xmldom *querytree) {
	return true;
}

const char *sqlrtranslation::getError() {
	return NULL;
}

sqlrtranslations *sqlrtranslation::getTranslations() {
	return pvt->_sqlts;
}

domnode *sqlrtranslation::getParameters() {
	return pvt->_parameters;
}

void sqlrtranslation::endTransaction(bool commit) {
}

void sqlrtranslation::endSession() {
}
