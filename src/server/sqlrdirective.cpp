// Copyright (c) 1999-2017  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrdirectiveprivate {
	friend class sqlrdirective;
	private:
		sqlrdirectives	*_sqlts;
		xmldomnode	*_parameters;
};

sqlrdirective::sqlrdirective(sqlrservercontroller *cont,
					sqlrdirectives *sqlts,
					xmldomnode *parameters) {
	pvt=new sqlrdirectiveprivate;
	pvt->_sqlts=sqlts;
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
	return pvt->_sqlts;
}

xmldomnode *sqlrdirective::getParameters() {
	return pvt->_parameters;
}
