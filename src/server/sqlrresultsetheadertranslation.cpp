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
					const char ***columnnames,
					uint16_t **columnnamelengths,
					uint16_t **columntypes,
					const char ***columntypenames,
					uint16_t **columntypenamelengths,
					uint32_t **columnlengths,
					uint32_t **columnprecisions,
					uint32_t **columnscales,
					uint16_t **columnisnullables,
					uint16_t **columnisprimarykeys,
					uint16_t **columnisuniques,
					uint16_t **columnispartofkeys,
					uint16_t **columnisunsigneds,
					uint16_t **columniszerofilleds,
					uint16_t **columnisbinarys,
					uint16_t **columnisautoincrements,
					const char ***columntables,
					uint16_t **columntablelengths) {
	return true;
}

sqlrresultsetheadertranslations *sqlrresultsetheadertranslation::
					getResultSetHeaderTranslations() {
	return pvt->_rs;
}

xmldomnode *sqlrresultsetheadertranslation::getParameters() {
	return pvt->_parameters;
}
