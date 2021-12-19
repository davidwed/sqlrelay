// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrquerytranslationprivate {
	friend class sqlrquerytranslation;
	private:

		sqlrquerytranslations	*_sqlts;
		domnode		*_parameters;
};

sqlrquerytranslation::sqlrquerytranslation(sqlrservercontroller *cont,
					sqlrquerytranslations *sqlts,
					domnode *parameters) {
	pvt=new sqlrquerytranslationprivate;
	pvt->_sqlts=sqlts;
	pvt->_parameters=parameters;
}

sqlrquerytranslation::~sqlrquerytranslation() {
	delete pvt;
}

bool sqlrquerytranslation::usesTree() {
	return false;
}

bool sqlrquerytranslation::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				const char *query,
				uint32_t querylength,
				stringbuffer *translatedquery) {
	return true;
}

bool sqlrquerytranslation::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				xmldom *querytree) {
	return true;
}

const char *sqlrquerytranslation::getError() {
	return NULL;
}

sqlrquerytranslations *sqlrquerytranslation::getQueryTranslations() {
	return pvt->_sqlts;
}

domnode *sqlrquerytranslation::getParameters() {
	return pvt->_parameters;
}

void sqlrquerytranslation::endTransaction(bool commit) {
}

void sqlrquerytranslation::endSession() {
}
