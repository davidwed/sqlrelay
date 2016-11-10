// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrresultsetrowtranslationprivate {
	friend class sqlrresultsetrowtranslation;
	private:
		xmldomnode			*_parameters;
};

sqlrresultsetrowtranslation::sqlrresultsetrowtranslation(
				sqlrservercontroller *cont,
				xmldomnode *parameters) {
	pvt=new sqlrresultsetrowtranslationprivate;
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

xmldomnode *sqlrresultsetrowtranslation::getParameters() {
	return pvt->_parameters;
}
