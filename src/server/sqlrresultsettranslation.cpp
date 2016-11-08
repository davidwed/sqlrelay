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
				sqlrresultsettranslations *sqlrrsts,
				xmldomnode *parameters,
				bool debug) {
	pvt=new sqlrresultsettranslationprivate;
	pvt->_sqlrrsts=sqlrrsts;
	pvt->_parameters=parameters;
	pvt->_debug=debug;
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

bool sqlrresultsettranslation::getDebug() {
	return pvt->_debug;
}
