// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrresultsetrowblocktranslationprivate {
	friend class sqlrresultsetrowblocktranslation;
	private:
		uint64_t	_rowblockcount;
};

sqlrresultsetrowblocktranslation::sqlrresultsetrowblocktranslation(
				sqlrservercontroller *cont,
				domnode *parameters) :
				sqlrservermodule(cont,parameters) {
	pvt=new sqlrresultsetrowblocktranslationprivate;

	pvt->_rowblockcount=charstring::convertToInteger(
		parameters->getParent()->getAttributeValue("rowblockcount"));
	if (pvt->_rowblockcount) {
		pvt->_rowblockcount=charstring::convertToInteger(
		parameters->getParent()->getAttributeValue("rowblocksize"));
	}
	if (!pvt->_rowblockcount) {
		pvt->_rowblockcount=10;
	}
}

sqlrresultsetrowblocktranslation::~sqlrresultsetrowblocktranslation() {
	delete pvt;
}

uint64_t sqlrresultsetrowblocktranslation::getRowBlockCount() {
	return pvt->_rowblockcount;
}

bool sqlrresultsetrowblocktranslation::setRow(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames,
						const char * const *fields,
						uint64_t *fieldsizes,
						bool *lobs,
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
						uint64_t **fieldsizes,
						bool **lobs,
						bool **nulls) {
	return true;
}

const char *sqlrresultsetrowblocktranslation::getError() {
	return NULL;
}
