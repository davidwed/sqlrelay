// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrresultsetrowtranslationprivate {
	friend class sqlrresultsetrowtranslation;
	private:
		sqlrresultsetrowtranslations	*_sqlrrrsts;
		xmldomnode			*_parameters;
		bool				_debug;
};

sqlrresultsetrowtranslation::sqlrresultsetrowtranslation(
				sqlrresultsetrowtranslations *sqlrrrsts,
				xmldomnode *parameters,
				bool debug) {
	pvt=new sqlrresultsetrowtranslationprivate;
	pvt->_sqlrrrsts=sqlrrrsts;
	pvt->_parameters=parameters;
	pvt->_debug=debug;
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

sqlrresultsetrowtranslations *sqlrresultsetrowtranslation::
					getResultSetRowTranslations() {
	return pvt->_sqlrrrsts;
}

xmldomnode *sqlrresultsetrowtranslation::getParameters() {
	return pvt->_parameters;
}

bool sqlrresultsetrowtranslation::getDebug() {
	return pvt->_debug;
}
