// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrresultsettranslationprivate {
	friend class sqlrresultsettranslation;
	private:
		sqlrresultsettranslations	*_rs;
		xmldomnode			*_parameters;
};

sqlrresultsettranslation::sqlrresultsettranslation(
				sqlrservercontroller *cont,
				sqlrresultsettranslations *rs,
				xmldomnode *parameters) {
	pvt=new sqlrresultsettranslationprivate;
	pvt->_rs=rs;
	pvt->_parameters=parameters;
}

sqlrresultsettranslation::~sqlrresultsettranslation() {
	delete pvt;
}

bool sqlrresultsettranslation::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint32_t fieldindex,
					const char **field,
					uint64_t *fieldlength) {
	return true;
}

sqlrresultsettranslations *sqlrresultsettranslation::
					getResultSetTranslations() {
	return pvt->_rs;
}

xmldomnode *sqlrresultsettranslation::getParameters() {
	return pvt->_parameters;
}
