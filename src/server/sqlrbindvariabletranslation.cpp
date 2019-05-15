// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrbindvariabletranslationprivate {
	friend class sqlrbindvariabletranslation;
	private:
		sqlrbindvariabletranslations	*_bvts;
		domnode				*_parameters;
};

sqlrbindvariabletranslation::sqlrbindvariabletranslation(
				sqlrservercontroller *cont,
				sqlrbindvariabletranslations *bvts,
				domnode *parameters) {
	pvt=new sqlrbindvariabletranslationprivate;
	pvt->_bvts=bvts;
	pvt->_parameters=parameters;
}

sqlrbindvariabletranslation::~sqlrbindvariabletranslation() {
	delete pvt;
}

bool sqlrbindvariabletranslation::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {
	return true;
}

const char *sqlrbindvariabletranslation::getError() {
	return NULL;
}

sqlrbindvariabletranslations *sqlrbindvariabletranslation::
					getBindVariableTranslations() {
	return pvt->_bvts;
}

domnode *sqlrbindvariabletranslation::getParameters() {
	return pvt->_parameters;
}

void sqlrbindvariabletranslation::endTransaction(bool commit) {
}

void sqlrbindvariabletranslation::endSession() {
}
