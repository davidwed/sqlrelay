// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrresultsetrowtranslationprivate {
	friend class sqlrresultsetrowtranslation;
	private:
		sqlrresultsetrowtranslations	*_rs;
		domnode			*_parameters;
};

sqlrresultsetrowtranslation::sqlrresultsetrowtranslation(
				sqlrservercontroller *cont,
				sqlrresultsetrowtranslations *rs,
				domnode *parameters) {
	pvt=new sqlrresultsetrowtranslationprivate;
	pvt->_rs=rs;
	pvt->_parameters=parameters;
}

sqlrresultsetrowtranslation::~sqlrresultsetrowtranslation() {
	delete pvt;
}

bool sqlrresultsetrowtranslation::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char ***field,
					uint64_t **fieldlength) {
	return true;
}

const char *sqlrresultsetrowtranslation::getError() {
	return NULL;
}

sqlrresultsetrowtranslations *sqlrresultsetrowtranslation::
					getResultSetRowTranslations() {
	return pvt->_rs;
}

domnode *sqlrresultsetrowtranslation::getParameters() {
	return pvt->_parameters;
}

void sqlrresultsetrowtranslation::endTransaction(bool commit) {
}

void sqlrresultsetrowtranslation::endSession() {
}
