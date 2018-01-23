// Copyright (c) 1999-2017  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrdirectiveprivate {
	friend class sqlrdirective;
	private:
		sqlrdirectives	*_sqlds;
		xmldomnode	*_parameters;
};

sqlrdirective::sqlrdirective(sqlrservercontroller *cont,
					sqlrdirectives *sqlds,
					xmldomnode *parameters) {
	pvt=new sqlrdirectiveprivate;
	pvt->_sqlds=sqlds;
	pvt->_parameters=parameters;
}

sqlrdirective::~sqlrdirective() {
	delete pvt;
}

bool sqlrdirective::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				const char *query) {
	return true;
}

sqlrdirectives *sqlrdirective::getDirectives() {
	return pvt->_sqlds;
}

xmldomnode *sqlrdirective::getParameters() {
	return pvt->_parameters;
}
