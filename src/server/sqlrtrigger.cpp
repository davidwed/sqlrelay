// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrtriggerprivate {
	friend class sqlrtrigger;
	private:
		xmldomnode	*_parameters;
};

sqlrtrigger::sqlrtrigger(sqlrservercontroller *cont, xmldomnode *parameters) {
	pvt=new sqlrtriggerprivate;
	pvt->_parameters=parameters;
}

sqlrtrigger::~sqlrtrigger() {
	delete pvt;
}

bool sqlrtrigger::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				xmldom *querytree,
				bool before,
				bool success) {
	return true;
}

xmldomnode *sqlrtrigger::getParameters() {
	return pvt->_parameters;
}
