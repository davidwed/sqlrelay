// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrresultsetheadertranslationprivate {
	friend class sqlrresultsetheadertranslation;
	private:
		sqlrresultsetheadertranslations	*_rs;
		xmldomnode			*_parameters;
};

sqlrresultsetheadertranslation::sqlrresultsetheadertranslation(
				sqlrservercontroller *cont,
				sqlrresultsetheadertranslations *rs,
				xmldomnode *parameters) {
	pvt=new sqlrresultsetheadertranslationprivate;
	pvt->_rs=rs;
	pvt->_parameters=parameters;
}

sqlrresultsetheadertranslation::~sqlrresultsetheadertranslation() {
	delete pvt;
}

bool sqlrresultsetheadertranslation::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char ***field,
					uint64_t **fieldlength) {
	return true;
}

sqlrresultsetheadertranslations *sqlrresultsetheadertranslation::
					getResultSetHeaderTranslations() {
	return pvt->_rs;
}

xmldomnode *sqlrresultsetheadertranslation::getParameters() {
	return pvt->_parameters;
}
