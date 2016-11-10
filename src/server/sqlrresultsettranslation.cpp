// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrresultsettranslationprivate {
	friend class sqlrresultsettranslation;
	private:
		sqlrresultsettranslations	*_sqlrrsts;
		xmldomnode			*_parameters;
		bool				_debug;
};

sqlrresultsettranslation::sqlrresultsettranslation(
				sqlrservercontroller *cont,
				sqlrresultsettranslations *sqlrrsts,
				xmldomnode *parameters) {
	pvt=new sqlrresultsettranslationprivate;
	pvt->_sqlrrsts=sqlrrsts;
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
	return pvt->_sqlrrsts;
}

xmldomnode *sqlrresultsettranslation::getParameters() {
	return pvt->_parameters;
}
