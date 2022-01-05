// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrerrortranslationprivate {
	friend class sqlrerrortranslation;
	private:

		sqlrerrortranslations	*_sqlets;
		domnode		*_parameters;
};

sqlrerrortranslation::sqlrerrortranslation(sqlrservercontroller *cont,
					sqlrerrortranslations *sqlets,
					domnode *parameters) {
	pvt=new sqlrerrortranslationprivate;
	pvt->_sqlets=sqlets;
	pvt->_parameters=parameters;
}

sqlrerrortranslation::~sqlrerrortranslation() {
	delete pvt;
}

bool sqlrerrortranslation::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				int64_t errornumber,
				const char *error,
				uint32_t errorlength,
				int64_t *translatederrornumber,
				const char **translatederror,
				uint32_t *translatederrorlength) {
	return true;
}

const char *sqlrerrortranslation::getError() {
	return NULL;
}

sqlrerrortranslations *sqlrerrortranslation::getErrorTranslations() {
	return pvt->_sqlets;
}

domnode *sqlrerrortranslation::getParameters() {
	return pvt->_parameters;
}

void sqlrerrortranslation::endTransaction(bool commit) {
}

void sqlrerrortranslation::endSession() {
}
