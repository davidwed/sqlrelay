// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrresultsetrowblocktranslationprivate {
	friend class sqlrresultsetrowblocktranslation;
	private:
		sqlrresultsetrowblocktranslations	*_rs;
		domnode					*_parameters;
};

sqlrresultsetrowblocktranslation::sqlrresultsetrowblocktranslation(
				sqlrservercontroller *cont,
				sqlrresultsetrowblocktranslations *rs,
				domnode *parameters) {
	pvt=new sqlrresultsetrowblocktranslationprivate;
	pvt->_rs=rs;
	pvt->_parameters=parameters;
}

sqlrresultsetrowblocktranslation::~sqlrresultsetrowblocktranslation() {
	delete pvt;
}

bool sqlrresultsetrowblocktranslation::setRow(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames,
						const char * const *fields,
						uint64_t *fieldlengths,
						bool *blobs,
						bool *nulls) {
	return true;
}

bool sqlrresultsetrowblocktranslation::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames) {
	return true;
}

bool sqlrresultsetrowblocktranslation::getRow(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char ***field,
						uint64_t **fieldlengths,
						bool **blobs,
						bool **nulls) {
	return true;
}

const char *sqlrresultsetrowblocktranslation::getError() {
	return NULL;
}

sqlrresultsetrowblocktranslations *sqlrresultsetrowblocktranslation::
					getResultSetRowBlockTranslations() {
	return pvt->_rs;
}

domnode *sqlrresultsetrowblocktranslation::getParameters() {
	return pvt->_parameters;
}

void sqlrresultsetrowblocktranslation::endTransaction(bool commit) {
}

void sqlrresultsetrowblocktranslation::endSession() {
}
